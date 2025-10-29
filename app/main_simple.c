/**
 * @file    main_simple.c
 * @brief   简化版测试 - Scheduler + HYlink解析器(不含UART)
 */

#include "main.h"
#include "board.h"
#include "scheduler.h"
#include "hylink_parser.h"

#include <string.h>

#if defined(STM32H743xx)
    #include "stm32h7xx.h"
#elif defined(STM32F407xx)
    #include "stm32f4xx.h"
#endif

/* ========================================================================
 * 测试数据
 * ======================================================================== */

/* 模拟接收到的HYlink数据包(心跳包) */
static const uint8_t test_packet[] = {
    0xBB, 0xAA,        /* SYNC_WORD */
    0x0C, 0x00,        /* LENGTH = 12 (11字节包头 + 1字节数据) */
    0x06,              /* DEVICE_ID = IO电路 */
    0x01,              /* SEQ_NUMBER */
    0x00,              /* CMD = 心跳包 */
    0x00,              /* RESERVED */
    0x7C, 0x4D,        /* DATA CRC16 (需要正确计算) */
    0xE5,              /* HEADER CHECKSUM (需要正确计算) */
    0x42,              /* DATA: 心跳计数 */
};

/* ========================================================================
 * 全局变量
 * ======================================================================== */

static volatile uint32_t g_packet_count = 0;

/* ========================================================================
 * HYlink回调
 * ======================================================================== */

void on_hylink_packet_received(const hylink_packet_t *packet)
{
    g_packet_count++;

    /* LED2闪烁表示收到数据包 */
    board_led_toggle(BOARD_LED_2);
}

/* ========================================================================
 * 任务定义
 * ======================================================================== */

/**
 * LED心跳任务 (优先级7)
 */
void task_led_heartbeat(void *param)
{
    (void)param;

    while (1) {
        board_led_toggle(BOARD_LED_1);
        sched_delay(500);
    }
}

/**
 * HYlink解析测试任务 (优先级6)
 * 每秒喂入一个测试数据包
 */
void task_hylink_test(void *param)
{
    (void)param;

    while (1) {
        /* 喂入测试数据包 */
        hylink_parser_feed(test_packet, sizeof(test_packet));

        sched_delay(1000);  /* 每1秒测试一次 */
    }
}

/**
 * 统计任务 (优先级4)
 */
void task_statistics(void *param)
{
    (void)param;
    hylink_parser_stats_t stats;

    while (1) {
        hylink_parser_get_stats(&stats);

        /* LED3闪烁表示统计输出 */
        board_led_toggle(BOARD_LED_3);

        /* 通过变量可以看到统计信息(调试时查看) */
        (void)stats;

        sched_delay(2000);
    }
}

/**
 * 空闲任务 (优先级0)
 */
void task_idle(void *param)
{
    (void)param;

    while (1) {
        __WFI();
    }
}

/* ========================================================================
 * 主函数
 * ======================================================================== */

int main(void)
{
    /* 1. 初始化硬件 */
    board_init();
    board_led_init();

    /* 2. 初始化HYlink解析器 */
    hylink_parser_init(on_hylink_packet_received);

    /* 3. 初始化调度器 */
    sched_init();

    /* 4. 创建任务 */
    sched_task_create(
        task_led_heartbeat,
        "LED_Heartbeat",
        512,
        NULL,
        7
    );

    sched_task_create(
        task_hylink_test,
        "HYlink_Test",
        1024,
        NULL,
        6
    );

    sched_task_create(
        task_statistics,
        "Statistics",
        512,
        NULL,
        4
    );

    sched_task_create(
        task_idle,
        "Idle",
        256,
        NULL,
        0
    );

    /* 5. 启动调度器 */
    sched_start();

    while (1);
}

void Error_Handler(void)
{
    board_fatal_halt();
}
