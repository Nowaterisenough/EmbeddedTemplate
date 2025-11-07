/**
 * @file    main.c
 * @brief   HYlink协议接收测试 - 结合Scheduler任务调度
 */

#include "main.h"
#include "board.h"
#include "scheduler.h"
#include "uart_driver.h"
#include "hylink_parser.h"
#include "version.h"

#include <stdio.h>
#include <string.h>

/* CMSIS头文件用于__WFI */
#if defined(STM32H743xx)
    #include "stm32h7xx.h"
#elif defined(STM32F407xx)
    #include "stm32f4xx.h"
#endif

/* ========================================================================
 * 全局变量
 * ======================================================================== */

/* 最近收到的数据包 */
static hylink_packet_t g_last_packet;
static volatile bool   g_packet_received = false;

/* 解析统计 */
static hylink_parser_stats_t g_stats;

/* ========================================================================
 * HYlink回调函数
 * ======================================================================== */

/**
 * HYlink数据包接收回调
 */
void on_hylink_packet_received(const hylink_packet_t *packet)
{
    /* 拷贝数据包(因为packet仅在回调期间有效) */
    memcpy(&g_last_packet, packet, sizeof(hylink_packet_t));
    g_packet_received = true;
}

/**
 * UART数据接收回调
 */
void on_uart_data_received(const uint8_t *data, uint16_t len)
{
    /* 喂给HYlink解析器 */
    hylink_parser_feed(data, len);
}

/* ========================================================================
 * 任务定义
 * ======================================================================== */

/**
 * LED闪烁任务 (优先级7 - 最高)
 * 用于指示系统运行状态
 */
void task_led_blink(void *param)
{
    (void)param;

    while (1) {
        board_led_toggle(BOARD_LED_1);
        sched_delay(500);  /* 500ms闪烁一次 */
    }
}

/**
 * HYlink协议处理任务 (优先级6)
 * 处理接收到的HYlink数据包
 */
void task_hylink_handler(void *param)
{
    (void)param;

    while (1) {
        if (g_packet_received) {
            /* 临界区保护 */
            sched_enter_critical();
            hylink_packet_t packet = g_last_packet;
            g_packet_received = false;
            sched_exit_critical();

            /* LED2短暂点亮表示收到数据包 */
            board_led_set(BOARD_LED_2, LED_ON);

            /* 处理不同类型的数据包 */
            switch (packet.header.cmd) {
                case CMD_HEARTBEAT:
                    /* 心跳包 */
                    break;

                case CMD_ATTITUDE_DATA:
                    /* 姿态数据 */
                    break;

                case CMD_POSITION_DATA:
                    /* 位置数据 */
                    break;

                default:
                    /* 其他命令 */
                    break;
            }

            sched_delay(50);
            board_led_set(BOARD_LED_2, LED_OFF);
        }

        sched_delay(10);  /* 每10ms检查一次 */
    }
}

/**
 * 统计信息任务 (优先级4)
 * 定期打印解析统计信息
 */
void task_statistics(void *param)
{
    (void)param;

    while (1) {
        /* 获取统计信息 */
        hylink_parser_get_stats(&g_stats);

        /* LED3闪烁表示统计输出 */
        board_led_toggle(BOARD_LED_3);

        /* 这里可以通过RTT或其他方式输出统计信息 */
        /* SEGGER_RTT_printf(0, "HYlink Stats: Total=%lu, CRC_Err=%lu, Hdr_Err=%lu\n",
                           g_stats.total_packets, g_stats.crc_errors, g_stats.header_errors); */

        sched_delay(2000);  /* 每2秒统计一次 */
    }
}

/**
 * 心跳发送任务 (优先级3)
 * 定期发送心跳包(可选)
 */
void task_heartbeat_send(void *param)
{
    (void)param;
    static uint8_t seq = 0;

    while (1) {
        /* 构造心跳包 */
        hylink_header_t header = {
            .sync_word_l = HYLINK_SYNC_WORD_L,
            .sync_word_h = HYLINK_SYNC_WORD_H,
            .device_id   = DEVICE_IO_CIRCUIT,  /* 本设备为IO电路 */
            .seq_number  = seq++,
            .cmd         = CMD_HEARTBEAT,
            .reserved    = 0,
        };

        /* 设置长度 (包头 + 1字节数据) */
        HYLINK_SET_LENGTH(&header, HYLINK_HEADER_SIZE + 1);

        /* 数据: 心跳计数 */
        uint8_t data[1] = {seq};

        /* 计算CRC */
        uint16_t crc = hylink_calc_crc16(data, 1);
        HYLINK_SET_DATA_CRC(&header, crc);

        /* 计算包头校验和 */
        header.check_header = hylink_calc_header_checksum(&header);

        /* 发送 */
        uart_send((uint8_t *)&header, sizeof(header));
        uart_send(data, 1);

        sched_delay(1000);  /* 1Hz心跳 */
    }
}

/**
 * 空闲任务 (优先级0 - 最低)
 */
void task_idle(void *param)
{
    (void)param;

    while (1) {
        __WFI();  /* 进入低功耗模式 */
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

    /* 打印固件版本信息 */
    version_print();

    /* 2. 初始化HYlink解析器 */
    hylink_parser_init(on_hylink_packet_received);

    /* 3. 初始化UART (230400波特率) */
    if (!uart_init(230400, on_uart_data_received)) {
        /* UART初始化失败,LED全亮报错 */
        board_led_set(BOARD_LED_1, LED_ON);
        board_led_set(BOARD_LED_2, LED_ON);
        board_led_set(BOARD_LED_3, LED_ON);
        while (1);
    }

    /* 4. 初始化调度器 */
    sched_init();

    /* 5. 创建任务 */
    sched_task_create(
        task_led_blink,
        "LED_Blink",
        512,
        NULL,
        7  /* 最高优先级 - 指示系统运行 */
    );

    sched_task_create(
        task_hylink_handler,
        "HYlink_Handler",
        1024,
        NULL,
        6  /* 高优先级 - 处理协议数据 */
    );

    sched_task_create(
        task_statistics,
        "Statistics",
        512,
        NULL,
        4  /* 中等优先级 - 统计信息 */
    );

    sched_task_create(
        task_heartbeat_send,
        "Heartbeat_TX",
        512,
        NULL,
        3  /* 中等优先级 - 发送心跳 */
    );

    sched_task_create(
        task_idle,
        "Idle",
        256,
        NULL,
        0  /* 最低优先级 - 空闲任务 */
    );

    /* 6. 启动调度器 (永不返回) */
    sched_start();

    /* 不应该执行到这里 */
    while (1);
}

void Error_Handler(void)
{
    board_fatal_halt();
}
