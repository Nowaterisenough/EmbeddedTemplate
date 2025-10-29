/**
 * @file    uart_driver.h
 * @brief   UART驱动抽象层 - DMA接收 + 空闲中断
 * @author  EmbeddedTemplate
 *
 * 设计原则:
 * - 硬件无关: 只暴露接收回调,不暴露HAL细节
 * - 零拷贝: DMA直接写入环形缓冲区
 * - 中断驱动: 空闲中断触发回调,不阻塞主循环
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * 配置参数
 * ======================================================================== */

#define UART_RX_BUFFER_SIZE  1024  /* DMA接收缓冲区大小 */

/* ========================================================================
 * 回调函数类型
 * ======================================================================== */

/**
 * UART接收回调
 *
 * @param data  接收到的数据指针
 * @param len   数据长度
 *
 * @note 在中断上下文中调用,应尽快返回
 * @note data指向的内存在回调返回后会被复用,需要立即拷贝或处理
 */
typedef void (*uart_rx_callback_t)(const uint8_t *data, uint16_t len);

/* ========================================================================
 * 驱动API
 * ======================================================================== */

/**
 * 初始化UART
 *
 * @param baudrate  波特率 (如 115200, 230400)
 * @param callback  接收回调函数
 * @return          true=成功, false=失败
 */
bool uart_init(uint32_t baudrate, uart_rx_callback_t callback);

/**
 * 发送数据 (阻塞式)
 *
 * @param data  数据指针
 * @param len   数据长度
 * @return      实际发送的字节数
 */
uint16_t uart_send(const uint8_t *data, uint16_t len);

/**
 * 检查UART是否就绪
 */
bool uart_is_ready(void);

/* ========================================================================
 * 内部接口 (由HAL层调用,用户不应直接调用)
 * ======================================================================== */

/**
 * UART空闲中断回调 (在stm32xxx_it.c中调用)
 *
 * @note 必须在UART空闲中断处理函数中调用此函数
 */
void uart_idle_callback(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_DRIVER_H */
