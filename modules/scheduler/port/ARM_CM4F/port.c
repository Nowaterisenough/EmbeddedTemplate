/**
 * @file    port.c
 * @brief   Cortex-M4F 移植层实现 (参考 FreeRTOS)
 */

#include "scheduler.h"

/* ========================================================================
 * Cortex-M4 寄存器定义
 * ======================================================================== */

#define NVIC_INT_CTRL_REG         (*((volatile uint32_t*)0xE000ED04))
#define NVIC_PENDSVSET_BIT        (1UL << 28UL)
#define NVIC_SYSPRI2_REG          (*((volatile uint32_t*)0xE000ED20))
#define NVIC_PENDSV_PRI           (0xFFUL << 16UL)  /* 最低优先级 */
#define NVIC_SYSTICK_PRI          (0xFFUL << 24UL)  /* 最低优先级 */

#define NVIC_SYSTICK_CTRL_REG     (*((volatile uint32_t*)0xE000E010))
#define NVIC_SYSTICK_LOAD_REG     (*((volatile uint32_t*)0xE000E014))
#define NVIC_SYSTICK_CURRENT_REG  (*((volatile uint32_t*)0xE000E018))
#define NVIC_SYSTICK_CLK_BIT      (1UL << 2UL)
#define NVIC_SYSTICK_INT_BIT      (1UL << 1UL)
#define NVIC_SYSTICK_ENABLE_BIT   (1UL << 0UL)

/* 初始值定义 */
#define INITIAL_XPSR              (0x01000000UL)    /* Thumb 位 */
#define INITIAL_EXC_RETURN        (0xFFFFFFFDUL)    /* 返回线程模式, 使用PSP */

/* ========================================================================
 * 外部引用
 * ======================================================================== */

extern void sched_switch_context(void);
extern void sched_tick_handler(void);
extern sched_stack_t** sched_get_current_stack_ptr(void);

/* ========================================================================
 * 栈初始化
 * ======================================================================== */

/**
 * 初始化任务栈
 *
 * Cortex-M4 异常栈帧布局 (硬件自动保存):
 *   xPSR
 *   PC (返回地址)
 *   LR
 *   R12
 *   R3
 *   R2
 *   R1
 *   R0 (参数)
 *
 * 软件保存的寄存器:
 *   R11
 *   R10
 *   R9
 *   R8
 *   R7
 *   R6
 *   R5
 *   R4
 *   EXC_RETURN
 *
 * FIX: LR 设置为 task_exit_error，防止任务函数返回时跳到 0x00
 */
sched_stack_t* port_init_stack(
    sched_stack_t  *stack_top,
    task_function_t task_func,
    void           *param
)
{
    /* 对齐到8字节边界 */
    stack_top = (sched_stack_t*)((uint32_t)stack_top & ~0x7UL);

    /* 模拟异常栈帧 (硬件自动保存部分) */
    *(--stack_top) = INITIAL_XPSR;                        /* xPSR */
    *(--stack_top) = (sched_stack_t)task_func;           /* PC */
    *(--stack_top) = (sched_stack_t)task_exit_error;     /* LR (FIX: 返回错误处理) */
    *(--stack_top) = 0;                                  /* R12 */
    *(--stack_top) = 0;                                  /* R3 */
    *(--stack_top) = 0;                                  /* R2 */
    *(--stack_top) = 0;                                  /* R1 */
    *(--stack_top) = (sched_stack_t)param;               /* R0 (参数) */

    /* 软件保存的寄存器 */
    *(--stack_top) = 0;                                  /* R11 */
    *(--stack_top) = 0;                                  /* R10 */
    *(--stack_top) = 0;                                  /* R9 */
    *(--stack_top) = 0;                                  /* R8 */
    *(--stack_top) = 0;                                  /* R7 */
    *(--stack_top) = 0;                                  /* R6 */
    *(--stack_top) = 0;                                  /* R5 */
    *(--stack_top) = 0;                                  /* R4 */
    *(--stack_top) = INITIAL_EXC_RETURN;                 /* EXC_RETURN */

    return stack_top;
}

/* ========================================================================
 * 启动第一个任务
 * ======================================================================== */

/**
 * 启动第一个任务 (naked 函数)
 *
 * 步骤：
 * 1. 设置 PSP 为第一个任务的栈指针
 * 2. 使能中断
 * 3. 触发 SVC 中断启动任务
 */
__attribute__((naked)) void port_start_first_task(void)
{
    __asm volatile (
        /* 获取当前任务栈指针 */
        "   ldr r0, =sched_get_current_stack_ptr   \n"
        "   bl  sched_get_current_stack_ptr         \n"
        "   ldr r1, [r0]                            \n"  /* r1 = *current_stack_ptr */
        "   ldr r0, [r1]                            \n"  /* r0 = **current_stack_ptr */

        /* 恢复软件保存的寄存器 */
        "   ldmia r0!, {r4-r11, r14}                \n"  /* 弹出 R4-R11, EXC_RETURN */

        /* 设置 PSP */
        "   msr psp, r0                             \n"
        "   isb                                     \n"

        /* 切换到 PSP */
        "   mrs r0, control                         \n"
        "   orr r0, r0, #2                          \n"  /* 使用 PSP */
        "   msr control, r0                         \n"
        "   isb                                     \n"

        /* 使能中断 */
        "   cpsie i                                 \n"
        "   cpsie f                                 \n"
        "   dsb                                     \n"
        "   isb                                     \n"

        /* 弹出硬件保存的寄存器, 开始执行任务 */
        "   bx r14                                  \n"
        "   .ltorg                                  \n"
    );
}

/* ========================================================================
 * 触发上下文切换
 * ======================================================================== */

void port_yield(void)
{
    /* 触发 PendSV 中断 */
    NVIC_INT_CTRL_REG = NVIC_PENDSVSET_BIT;

    /* 内存屏障 */
    __asm volatile ("dsb" ::: "memory");
    __asm volatile ("isb");
}

/* ========================================================================
 * PendSV 中断处理 (上下文切换)
 * ======================================================================== */

/**
 * PendSV 中断处理 (naked 函数)
 *
 * 步骤：
 * 1. 保存当前任务上下文
 * 2. 调用调度器选择下一个任务
 * 3. 恢复新任务上下文
 *
 * FIX: 保存 PSP 到临时寄存器，避免被函数调用覆盖
 */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile (
        /* 获取当前 PSP (修改前的栈指针) */
        "   mrs r0, psp                             \n"
        "   isb                                     \n"

        /* 保存软件寄存器 (R4-R11, EXC_RETURN) */
        "   stmdb r0!, {r4-r11, r14}                \n"

        /* 保存 PSP 到 r1 (防止函数调用覆盖 r0) */
        "   mov r1, r0                              \n"

        /* 保存当前任务栈指针到 TCB */
        "   bl  sched_get_current_stack_ptr         \n"  /* r0 = &current_task->stack_ptr */
        "   ldr r2, [r0]                            \n"  /* r2 = current_task (TCB地址) */
        "   str r1, [r2]                            \n"  /* TCB->stack_ptr = r1 (修改后的PSP) */

        /* 调用调度器 */
        "   bl sched_switch_context                 \n"

        /* 获取新任务栈指针 */
        "   bl  sched_get_current_stack_ptr         \n"  /* r0 = &current_task->stack_ptr */
        "   ldr r1, [r0]                            \n"  /* r1 = current_task (新TCB) */
        "   ldr r0, [r1]                            \n"  /* r0 = current_task->stack_ptr (新PSP) */

        /* 恢复软件寄存器 */
        "   ldmia r0!, {r4-r11, r14}                \n"

        /* 恢复 PSP */
        "   msr psp, r0                             \n"
        "   isb                                     \n"

        /* 返回 */
        "   bx r14                                  \n"
        "   .ltorg                                  \n"
    );
}

/* ========================================================================
 * SysTick 中断处理
 * ======================================================================== */

void SysTick_Handler(void)
{
    /* 调用调度器滴答处理 */
    sched_tick_handler();
}

/* ========================================================================
 * SysTick 配置
 * ======================================================================== */

void port_setup_systick(uint32_t tick_rate_hz)
{
    extern uint32_t SystemCoreClock;  /* CMSIS 定义的系统时钟 */

    /* 停止 SysTick */
    NVIC_SYSTICK_CTRL_REG = 0UL;
    NVIC_SYSTICK_CURRENT_REG = 0UL;

    /* 配置 reload 值 */
    uint32_t reload = (SystemCoreClock / tick_rate_hz) - 1UL;
    NVIC_SYSTICK_LOAD_REG = reload;

    /* 设置 PendSV 和 SysTick 为最低优先级 */
    NVIC_SYSPRI2_REG |= NVIC_PENDSV_PRI | NVIC_SYSTICK_PRI;

    /* 启动 SysTick */
    NVIC_SYSTICK_CTRL_REG = (NVIC_SYSTICK_CLK_BIT |
                             NVIC_SYSTICK_INT_BIT |
                             NVIC_SYSTICK_ENABLE_BIT);
}
