/**
 * @file    scheduler.h
 * @brief   轻量级抢占式任务调度器 (参考 FreeRTOS 设计)
 * @author  EmbeddedTemplate
 *
 * 核心特性：
 * - 抢占式优先级调度
 * - 时间片轮转 (相同优先级)
 * - 最小栈开销
 * - Cortex-M 优化
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * 配置参数
 * ======================================================================== */

#define SCHED_MAX_PRIORITIES        8      /* 支持的最大优先级数 (0-7, 7最高) */
#define SCHED_MAX_TASKS             16     /* 最大任务数 */
#define SCHED_TICK_RATE_HZ          1000   /* 系统滴答频率 (1ms) */
#define SCHED_TIME_SLICE_TICKS      10     /* 时间片长度 (10ms) */
#define SCHED_MIN_STACK_SIZE        256    /* 最小栈大小 (字节) */

/* ========================================================================
 * 类型定义
 * ======================================================================== */

typedef uint32_t sched_tick_t;
typedef uint32_t sched_stack_t;  /* 栈单元类型 (32-bit) */

typedef enum {
    TASK_READY = 0,              /* 就绪态 */
    TASK_RUNNING,                /* 运行态 */
    TASK_BLOCKED,                /* 阻塞态 */
    TASK_SUSPENDED,              /* 挂起态 */
    TASK_DELETED                 /* 已删除 */
} task_state_t;

typedef void (*task_function_t)(void *param);

/**
 * 任务控制块 (TCB)
 * 参考 FreeRTOS 设计，但更简化
 */
typedef struct task_control_block {
    sched_stack_t      *stack_ptr;         /* 当前栈指针 */
    sched_stack_t      *stack_base;        /* 栈底 */
    uint32_t            stack_size;        /* 栈大小 */

    task_function_t     task_func;         /* 任务函数 */
    void               *param;             /* 任务参数 */

    uint8_t             priority;          /* 优先级 (0-7) */
    task_state_t        state;             /* 任务状态 */

    sched_tick_t        time_slice;        /* 剩余时间片 */
    sched_tick_t        block_time;        /* 阻塞超时时间 */

    const char         *name;              /* 任务名称 (调试用) */

    struct task_control_block *next;       /* 链表指针 */
} tcb_t;

/**
 * 任务句柄 (对外不透明)
 */
typedef tcb_t* task_handle_t;

/* ========================================================================
 * 调度器 API
 * ======================================================================== */

/**
 * 初始化调度器
 */
void sched_init(void);

/**
 * 启动调度器 (永不返回)
 */
void sched_start(void);

/**
 * 创建任务
 *
 * @param task_func  任务函数
 * @param name       任务名称
 * @param stack_size 栈大小 (字节)
 * @param param      任务参数
 * @param priority   优先级 (0-7, 7最高)
 * @return           任务句柄, NULL表示失败
 */
task_handle_t sched_task_create(
    task_function_t task_func,
    const char     *name,
    uint32_t        stack_size,
    void           *param,
    uint8_t         priority
);

/**
 * 删除任务
 */
void sched_task_delete(task_handle_t task);

/**
 * 任务主动让出 CPU (触发上下文切换)
 */
void sched_yield(void);

/**
 * 任务延时 (阻塞指定时间)
 *
 * @param ticks 延时的滴答数
 */
void sched_delay(sched_tick_t ticks);

/**
 * 获取系统滴答计数
 */
sched_tick_t sched_get_tick_count(void);

/**
 * 获取当前运行任务
 */
task_handle_t sched_get_current_task(void);

/**
 * 进入临界区 (禁止中断)
 */
void sched_enter_critical(void);

/**
 * 退出临界区 (使能中断)
 */
void sched_exit_critical(void);

/* ========================================================================
 * 移植层接口 (port.c 实现)
 * ======================================================================== */

/**
 * 初始化任务栈
 *
 * @param stack_top   栈顶地址
 * @param task_func   任务函数
 * @param param       任务参数
 * @return            初始化后的栈指针
 */
sched_stack_t* port_init_stack(
    sched_stack_t  *stack_top,
    task_function_t task_func,
    void           *param
);

/**
 * 启动第一个任务
 */
void port_start_first_task(void);

/**
 * 触发PendSV中断 (上下文切换)
 */
void port_yield(void);

/**
 * 设置SysTick定时器
 */
void port_setup_systick(uint32_t tick_rate_hz);

/* ========================================================================
 * 中断处理函数 (在 port.c 中定义为 naked 函数)
 * ======================================================================== */

/**
 * PendSV 中断处理 (上下文切换)
 * 必须在中断向量表中注册
 */
void PendSV_Handler(void);

/**
 * SysTick 中断处理 (系统滴答)
 * 必须在中断向量表中注册
 */
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULER_H */
