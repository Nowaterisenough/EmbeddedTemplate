/**
 * @file    scheduler.c
 * @brief   轻量级抢占式任务调度器核心实现
 */

#include "scheduler.h"
#include <string.h>

/* CMSIS 内联函数 */
#include "cmsis_compiler.h"

/* ========================================================================
 * 内部数据结构
 * ======================================================================== */

/* 任务栈池：静态分配在 DTCM RAM 的专用区域 */
static uint8_t task_stack_pool[SCHED_MAX_TASKS][SCHED_DEFAULT_STACK_SIZE]
    __attribute__((section(".task_stacks"), aligned(8)));

/* 栈分配位图 */
static uint16_t stack_allocated_bitmap = 0;

/* 就绪队列：每个优先级一个链表 */
static tcb_t *ready_queue[SCHED_MAX_PRIORITIES] = {NULL};

/* 任务池 */
static tcb_t task_pool[SCHED_MAX_TASKS];

/* 空闲 TCB 链表头 (用于回收) */
static tcb_t *free_tcb_list = NULL;

/* 已分配的任务数量 (用于快速查找上限) */
static uint32_t allocated_task_count = 0;

/* 当前运行任务 */
static tcb_t *current_task = NULL;

/* 系统滴答计数 */
static volatile sched_tick_t tick_count = 0;

/* 临界区嵌套计数 */
static volatile uint32_t critical_nesting = 0;

/* 调度器状态 */
static bool scheduler_running = false;

/* 优先级位图 (用于快速查找最高优先级) */
static uint8_t ready_priority_bitmap = 0;

/* ========================================================================
 * 内部辅助函数
 * ======================================================================== */

/**
 * 从栈池分配栈空间
 * @return 栈索引，-1 表示失败
 */
static int allocate_stack_from_pool(void)
{
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        if ((stack_allocated_bitmap & (1 << i)) == 0) {
            stack_allocated_bitmap |= (1 << i);
            return i;
        }
    }
    return -1;  /* 栈池已满 */
}

/**
 * 释放栈回池
 * @param stack_index 栈索引
 */
static void free_stack_to_pool(int stack_index)
{
    if (stack_index >= 0 && stack_index < SCHED_MAX_TASKS) {
        stack_allocated_bitmap &= ~(1 << stack_index);
    }
}

/**
 * 将任务添加到就绪队列
 */
static void add_task_to_ready_queue(tcb_t *task)
{
    if (!task) return;

    uint8_t prio = task->priority;
    task->state = TASK_READY;

    /* 添加到对应优先级的队列尾部 */
    if (ready_queue[prio] == NULL) {
        ready_queue[prio] = task;
        task->next = task;  /* 循环链表 */
    } else {
        /* 插入到队列尾部 */
        tcb_t *tail = ready_queue[prio];
        while (tail->next != ready_queue[prio]) {
            tail = tail->next;
        }
        tail->next = task;
        task->next = ready_queue[prio];
    }

    /* 更新优先级位图 */
    ready_priority_bitmap |= (1 << prio);
}

/**
 * 从就绪队列移除任务
 */
static void remove_task_from_ready_queue(tcb_t *task)
{
    if (!task) return;

    uint8_t prio = task->priority;
    tcb_t *head = ready_queue[prio];

    if (head == NULL) return;

    /* 单节点情况 */
    if (head->next == head) {
        if (head == task) {
            ready_queue[prio] = NULL;
            ready_priority_bitmap &= ~(1 << prio);
        }
        return;
    }

    /* 多节点情况 */
    tcb_t *prev = head;
    while (prev->next != task && prev->next != head) {
        prev = prev->next;
    }

    if (prev->next == task) {
        prev->next = task->next;
        if (task == head) {
            ready_queue[prio] = task->next;
        }

        /* 如果队列为空，清除位图 */
        if (ready_queue[prio]->next == ready_queue[prio]) {
            /* 仍有一个节点，不清除 */
        }
    }
}

/**
 * 选择下一个运行的任务 (最高优先级)
 */
static tcb_t* select_next_task(void)
{
    /* 使用位图快速查找最高优先级 */
    if (ready_priority_bitmap == 0) {
        return NULL;  /* 空闲 */
    }

    /* 找到最高优先级 (从高到低扫描) */
    for (int i = SCHED_MAX_PRIORITIES - 1; i >= 0; i--) {
        if (ready_priority_bitmap & (1 << i)) {
            tcb_t *task = ready_queue[i];

            /* 时间片轮转：选择队列头，然后移动到下一个 */
            if (task && task->next != task) {
                ready_queue[i] = task->next;
            }

            return task;
        }
    }

    return NULL;
}

/**
 * 任务退出错误处理
 * FIX: 声明为非 static，供移植层使用
 */
void task_exit_error(void)
{
    /* 任务不应该退出，如果执行到这里说明出错了 */
    sched_enter_critical();
    while (1) {
        /* 死循环 */
    }
}

/* ========================================================================
 * 公共 API 实现
 * ======================================================================== */

void sched_init(void)
{
    /* 清空所有数据结构 */
    memset(task_pool, 0, sizeof(task_pool));
    memset(ready_queue, 0, sizeof(ready_queue));

    /* 初始化 TCB 空闲链表 */
    for (uint32_t i = 0; i < SCHED_MAX_TASKS; i++) {
        task_pool[i].next = (i < SCHED_MAX_TASKS - 1) ? &task_pool[i + 1] : NULL;
    }
    free_tcb_list = &task_pool[0];
    allocated_task_count = 0;

    current_task = NULL;
    tick_count = 0;
    critical_nesting = 0;
    scheduler_running = false;
    ready_priority_bitmap = 0;
    stack_allocated_bitmap = 0;  /* 清空栈分配位图 */
}

task_handle_t sched_task_create(
    task_function_t task_func,
    const char     *name,
    uint32_t        stack_size,
    void           *param,
    uint8_t         priority
)
{
    if (priority >= SCHED_MAX_PRIORITIES) return NULL;

    /* 从栈池分配栈空间 */
    int stack_index = allocate_stack_from_pool();
    if (stack_index < 0) {
        return NULL;  /* 栈池已满 */
    }

    /* 从空闲 TCB 链表分配 TCB (FIX: 支持TCB回收) */
    sched_enter_critical();
    if (free_tcb_list == NULL) {
        sched_exit_critical();
        free_stack_to_pool(stack_index);
        return NULL;  /* 没有空闲 TCB */
    }

    tcb_t *task = free_tcb_list;
    free_tcb_list = free_tcb_list->next;
    allocated_task_count++;
    sched_exit_critical();

    /* 使用预分配的栈 */
    task->stack_base = (sched_stack_t*)task_stack_pool[stack_index];
    task->stack_size = SCHED_DEFAULT_STACK_SIZE;

    /* 初始化 TCB */
    task->task_func = task_func;
    task->param = param;
    task->priority = priority;
    task->state = TASK_READY;
    task->time_slice = SCHED_TIME_SLICE_TICKS;
    task->block_time = 0;
    task->name = name;
    task->next = NULL;

    /* 初始化任务栈 (调用移植层) */
    sched_stack_t *stack_top = task->stack_base + (SCHED_DEFAULT_STACK_SIZE / sizeof(sched_stack_t)) - 1;
    task->stack_ptr = port_init_stack(stack_top, task_func, param);

    /* 添加到就绪队列 */
    add_task_to_ready_queue(task);

    return task;
}

void sched_task_delete(task_handle_t task)
{
    if (!task) return;

    sched_enter_critical();

    /* 从就绪队列移除 */
    remove_task_from_ready_queue(task);

    /* 释放栈回池 */
    if (task->stack_base) {
        /* 计算栈索引 */
        uintptr_t stack_offset = (uintptr_t)task->stack_base - (uintptr_t)task_stack_pool;
        int stack_index = stack_offset / SCHED_DEFAULT_STACK_SIZE;

        /* 释放栈 */
        free_stack_to_pool(stack_index);
        task->stack_base = NULL;
    }

    task->state = TASK_DELETED;

    /* FIX: 将 TCB 放回空闲链表，支持任务回收 */
    task->next = free_tcb_list;
    free_tcb_list = task;
    allocated_task_count--;

    sched_exit_critical();

    /* 如果删除当前任务，立即切换 */
    if (task == current_task) {
        sched_yield();
    }
}

void sched_start(void)
{
    if (allocated_task_count == 0) {
        /* 没有任务，无法启动 */
        return;
    }

    scheduler_running = true;

    /* 配置SysTick */
    port_setup_systick(SCHED_TICK_RATE_HZ);

    /* 选择第一个任务 */
    current_task = select_next_task();
    if (current_task) {
        current_task->state = TASK_RUNNING;
    }

    /* 启动第一个任务 (永不返回) */
    port_start_first_task();
}

void sched_yield(void)
{
    /* 触发PendSV中断进行上下文切换 */
    port_yield();
}

void sched_delay(sched_tick_t ticks)
{
    if (ticks == 0 || !scheduler_running) return;

    sched_enter_critical();

    /* 设置阻塞时间 */
    current_task->block_time = tick_count + ticks;
    current_task->state = TASK_BLOCKED;

    /* 从就绪队列移除 */
    remove_task_from_ready_queue(current_task);

    sched_exit_critical();

    /* 触发调度 */
    sched_yield();
}

sched_tick_t sched_get_tick_count(void)
{
    return tick_count;
}

task_handle_t sched_get_current_task(void)
{
    return current_task;
}

void sched_enter_critical(void)
{
    __disable_irq();
    critical_nesting++;
}

void sched_exit_critical(void)
{
    if (critical_nesting > 0) {
        critical_nesting--;
        if (critical_nesting == 0) {
            __enable_irq();
        }
    }
}

/* ========================================================================
 * 调度器内部函数 (由中断调用)
 * ======================================================================== */

/**
 * 任务切换逻辑 (在 PendSV 中调用)
 */
void sched_switch_context(void)
{
    /* 保存当前任务的栈指针已在汇编中完成 */

    /* 选择下一个任务 */
    tcb_t *next_task = select_next_task();

    if (next_task) {
        /* 更新当前任务状态 */
        if (current_task && current_task->state == TASK_RUNNING) {
            current_task->state = TASK_READY;
        }

        /* 切换到新任务 */
        current_task = next_task;
        current_task->state = TASK_RUNNING;
        current_task->time_slice = SCHED_TIME_SLICE_TICKS;
    }

    /* 恢复新任务的栈指针在汇编中完成 */
}

/**
 * 系统滴答处理 (在 SysTick 中调用)
 *
 * FIX: 添加立即抢占逻辑 - 当高优先级任务被唤醒时立即切换
 */
void sched_tick_handler(void)
{
    tick_count++;

    bool need_schedule = false;

    /* 检查阻塞任务是否超时 */
    for (uint32_t i = 0; i < SCHED_MAX_TASKS; i++) {
        tcb_t *task = &task_pool[i];
        if (task->state == TASK_BLOCKED) {
            /* 使用溢出安全的比较：(int32_t)差值 >= 0 */
            int32_t diff = (int32_t)(tick_count - task->block_time);
            if (diff >= 0) {
                /* 超时，恢复到就绪队列 */
                add_task_to_ready_queue(task);

                /* FIX: 如果被唤醒的任务优先级更高，立即抢占 */
                if (current_task && task->priority > current_task->priority) {
                    need_schedule = true;
                }
            }
        }
    }

    /* 时间片递减 */
    if (current_task && current_task->time_slice > 0) {
        current_task->time_slice--;

        /* 时间片耗尽，触发调度 */
        if (current_task->time_slice == 0) {
            need_schedule = true;
        }
    }

    /* 如果需要调度则触发 */
    if (need_schedule) {
        sched_yield();
    }
}

/**
 * 获取当前任务的栈指针 (用于上下文切换)
 */
sched_stack_t** sched_get_current_stack_ptr(void)
{
    if (current_task) {
        return &(current_task->stack_ptr);
    }
    return NULL;
}
