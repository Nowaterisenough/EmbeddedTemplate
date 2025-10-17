/**
 * @file    scheduler_demo.c
 * @brief   调度器示例代码
 *
 * 本示例演示：
 * 1. 多任务创建
 * 2. 优先级抢占
 * 3. 时间片轮转
 * 4. 任务延时
 */

#include "scheduler.h"
#include "board.h"

/* ========================================================================
 * 示例任务定义
 * ======================================================================== */

/**
 * 高优先级任务 (优先级 7)
 * 每 500ms 运行一次，点亮 LED1
 */
void task_high_priority(void *param)
{
    (void)param;

    while (1) {
        board_led_set(BOARD_LED_1, LED_ON);
        sched_delay(50);   /* 点亮 50ms */

        board_led_set(BOARD_LED_1, LED_OFF);
        sched_delay(450);  /* 熄灭 450ms */
    }
}

/**
 * 中等优先级任务 (优先级 5)
 * 每 1000ms 运行一次，点亮 LED2
 */
void task_medium_priority(void *param)
{
    (void)param;

    while (1) {
        board_led_set(BOARD_LED_2, LED_ON);
        sched_delay(100);   /* 点亮 100ms */

        board_led_set(BOARD_LED_2, LED_OFF);
        sched_delay(900);   /* 熄灭 900ms */
    }
}

/**
 * 低优先级任务1 (优先级 3)
 * 与任务2同优先级，演示时间片轮转
 */
void task_low_priority_1(void *param)
{
    (void)param;
    uint32_t counter = 0;

    while (1) {
        counter++;

        /* 每 500ms 翻转一次 */
        if (counter % 500 == 0) {
            board_led_toggle(BOARD_LED_3);
        }

        sched_delay(1);  /* 每 1ms 执行一次 */
    }
}

/**
 * 低优先级任务2 (优先级 3)
 * 与任务1同优先级，演示时间片轮转
 */
void task_low_priority_2(void *param)
{
    (void)param;
    uint32_t last_tick = 0;

    while (1) {
        uint32_t current_tick = sched_get_tick_count();

        /* 每秒打印一次 (如果有 RTT) */
        if (current_tick - last_tick >= 1000) {
            last_tick = current_tick;
            /* SEGGER_RTT_printf(0, "Tick: %lu\n", current_tick); */
        }

        sched_delay(10);
    }
}

/**
 * 空闲任务 (优先级 0)
 * CPU 空闲时运行，可以进入低功耗模式
 */
void task_idle(void *param)
{
    (void)param;

    while (1) {
        /* 空闲时可以执行：
         * - 低功耗模式 (WFI)
         * - 看门狗喂狗
         * - 统计 CPU 使用率
         */
        __WFI();  /* 等待中断 */
    }
}

/* ========================================================================
 * 主函数
 * ======================================================================== */

int main_scheduler_demo(void)
{
    /* 1. 初始化硬件 */
    board_init();
    board_led_init();

    /* 2. 初始化调度器 */
    sched_init();

    /* 3. 创建任务 */
    sched_task_create(
        task_high_priority,
        "HighPrio",
        512,
        NULL,
        7  /* 最高优先级 */
    );

    sched_task_create(
        task_medium_priority,
        "MediumPrio",
        512,
        NULL,
        5
    );

    sched_task_create(
        task_low_priority_1,
        "LowPrio1",
        512,
        NULL,
        3  /* 同优先级，时间片轮转 */
    );

    sched_task_create(
        task_low_priority_2,
        "LowPrio2",
        512,
        NULL,
        3  /* 同优先级，时间片轮转 */
    );

    sched_task_create(
        task_idle,
        "Idle",
        256,
        NULL,
        0  /* 最低优先级 */
    );

    /* 4. 启动调度器 (永不返回) */
    sched_start();

    /* 不应该执行到这里 */
    while (1);
}

/* ========================================================================
 * 高级示例：动态创建/删除任务
 * ======================================================================== */

void task_manager(void *param)
{
    (void)param;

    task_handle_t temp_task = NULL;
    uint32_t create_time = 0;

    while (1) {
        uint32_t current_tick = sched_get_tick_count();

        /* 每 5 秒创建一个临时任务 */
        if (current_tick - create_time >= 5000 && temp_task == NULL) {
            temp_task = sched_task_create(
                task_medium_priority,
                "TempTask",
                512,
                NULL,
                4
            );
            create_time = current_tick;
        }

        /* 临时任务运行 2 秒后删除 */
        if (temp_task && (current_tick - create_time >= 2000)) {
            sched_task_delete(temp_task);
            temp_task = NULL;
        }

        sched_delay(100);
    }
}
