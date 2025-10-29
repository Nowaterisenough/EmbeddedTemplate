/**
 * @file    uart_driver.h
 * @brief   UART驱动抽象接口 - 硬件无关
 * @author  EmbeddedTemplate
 *
 * 设计原则:
 * - 完全硬件无关：不包含任何芯片特定的头文件或类型
 * - 接口简洁：只暴露应用层需要的功能
 * - 回调机制：通过回调函数通知上层数据到达
 *
 * 实现说明:
 * - 具体实现由板级代码提供（boards/xxx/uart_port.c）
 * - 建议使用 DMA + 空闲中断实现零拷贝接收
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * 类型定义
 * ======================================================================== */

/**
 * UART接收回调函数类型
 *
 * @param data  接收到的数据缓冲区
 * @param len   数据长度（字节）
 *
 * @note 此函数在中断上下文中调用，应尽快处理并返回
 * @note data 指向的内存可能在回调返回后被复用，需立即处理或拷贝
 */
typedef void (*uart_rx_callback_t)(const uint8_t *data, uint16_t len);

/* ========================================================================
 * 公共接口
 * ======================================================================== */

/**
 * 初始化 UART 驱动
 *
 * @param baudrate  波特率（如 115200）
 * @param callback  接收数据回调函数，不能为 NULL
 * @return          true=初始化成功，false=失败
 *
 * @note 必须在使用其他 UART 函数之前调用
 * @note 实际硬件配置（GPIO、DMA等）由板级代码完成
 */
bool uart_init(uint32_t baudrate, uart_rx_callback_t callback);

/**
 * 通过 UART 发送数据（阻塞）
 *
 * @param data  待发送数据
 * @param len   数据长度（字节）
 * @return      实际发送的字节数
 *
 * @note 阻塞等待发送完成，不适合在中断中调用
 */
uint16_t uart_send(const uint8_t *data, uint16_t len);

/**
 * 检查 UART 是否就绪
 *
 * @return  true=就绪，false=忙碌
 */
bool uart_is_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_DRIVER_H */
