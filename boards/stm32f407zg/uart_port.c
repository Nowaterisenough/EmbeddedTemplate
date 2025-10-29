/**
 * @file    uart_port.c
 * @brief   STM32F407ZG UART 硬件实现
 * @note    使用 USART1 (PA9=TX, PA10=RX) + DMA2_Stream5
 *
 * 实现说明：
 * - DMA 循环接收，零拷贝设计
 * - 空闲中断触发数据处理
 * - 环形缓冲区自动处理回绕
 */

#include "uart_driver.h"
#include "stm32f4xx_hal.h"

/* ========================================================================
 * 配置参数
 * ======================================================================== */

#define UART_RX_BUFFER_SIZE  1024    /* DMA 接收缓冲区大小 */
#define UART_INSTANCE        USART1  /* 使用 USART1 */

/* ========================================================================
 * 私有变量
 * ======================================================================== */

static UART_HandleTypeDef huart;              /* UART 句柄 */
static DMA_HandleTypeDef  hdma_rx;            /* DMA 接收句柄 */
static uint8_t            rx_buffer[UART_RX_BUFFER_SIZE];  /* 接收缓冲区 */
static uart_rx_callback_t rx_callback = NULL; /* 接收回调 */
static uint16_t           last_rx_pos = 0;    /* 上次 DMA 位置 */

/* ========================================================================
 * 私有函数
 * ======================================================================== */

/**
 * 处理接收到的数据（环形缓冲区）
 */
static void process_received_data(void)
{
    if (!rx_callback) {
        return;
    }

    /* 获取当前 DMA 传输位置 */
    uint16_t current_pos = UART_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart.hdmarx);

    if (current_pos == last_rx_pos) {
        return;  /* 没有新数据 */
    }

    uint16_t data_len;
    const uint8_t *data_ptr;

    if (current_pos > last_rx_pos) {
        /* 正常情况: [last_rx_pos, current_pos) */
        data_ptr = &rx_buffer[last_rx_pos];
        data_len = current_pos - last_rx_pos;
        rx_callback(data_ptr, data_len);
    } else {
        /* 缓冲区回绕: [last_rx_pos, end) + [0, current_pos) */
        /* 先处理第一段 */
        data_ptr = &rx_buffer[last_rx_pos];
        data_len = UART_RX_BUFFER_SIZE - last_rx_pos;
        rx_callback(data_ptr, data_len);

        /* 再处理第二段 */
        if (current_pos > 0) {
            data_ptr = &rx_buffer[0];
            data_len = current_pos;
            rx_callback(data_ptr, data_len);
        }
    }

    last_rx_pos = current_pos;
}

/**
 * UART 空闲中断回调（内部使用）
 */
static void uart_idle_callback(void)
{
    /* 清除 IDLE 标志 */
    __HAL_UART_CLEAR_IDLEFLAG(&huart);

    /* 处理接收数据 */
    process_received_data();
}

/**
 * STM32F4 UART 硬件初始化
 *
 * @param baudrate  波特率
 * @return          true=成功, false=失败
 */
static bool hal_uart_init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* 配置 GPIO: PA9=TX, PA10=RX */
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 配置 UART */
    huart.Instance          = UART_INSTANCE;
    huart.Init.BaudRate     = baudrate;
    huart.Init.WordLength   = UART_WORDLENGTH_8B;
    huart.Init.StopBits     = UART_STOPBITS_1;
    huart.Init.Parity       = UART_PARITY_NONE;
    huart.Init.Mode         = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart) != HAL_OK) {
        return false;
    }

    /* 配置 DMA: DMA2_Stream5 用于 USART1_RX */
    hdma_rx.Instance                 = DMA2_Stream5;
    hdma_rx.Init.Channel             = DMA_CHANNEL_4;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx.Init.Mode                = DMA_CIRCULAR;
    hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&hdma_rx) != HAL_OK) {
        return false;
    }

    __HAL_LINKDMA(&huart, hdmarx, hdma_rx);

    /* 使能 UART 空闲中断 */
    __HAL_UART_ENABLE_IT(&huart, UART_IT_IDLE);

    /* 配置 NVIC 优先级 */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

    return true;
}

/* ========================================================================
 * 公共接口实现（实现 uart_driver.h）
 * ======================================================================== */

bool uart_init(uint32_t baudrate, uart_rx_callback_t callback)
{
    if (!callback) {
        return false;
    }

    rx_callback = callback;
    last_rx_pos = 0;

    if (!hal_uart_init(baudrate)) {
        return false;
    }

    /* 启动 DMA 接收 */
    HAL_UART_Receive_DMA(&huart, rx_buffer, UART_RX_BUFFER_SIZE);

    return true;
}

uint16_t uart_send(const uint8_t *data, uint16_t len)
{
    if (HAL_UART_Transmit(&huart, (uint8_t *)data, len, 1000) == HAL_OK) {
        return len;
    }
    return 0;
}

bool uart_is_ready(void)
{
    return (huart.gState == HAL_UART_STATE_READY);
}

/* ========================================================================
 * 中断服务函数
 * ======================================================================== */

/**
 * USART1 中断处理
 */
void USART1_IRQHandler(void)
{
    /* 检查是否是空闲中断 */
    if (__HAL_UART_GET_FLAG(&huart, UART_FLAG_IDLE)) {
        uart_idle_callback();
    }

    HAL_UART_IRQHandler(&huart);
}

/**
 * DMA2_Stream5 中断处理
 */
void DMA2_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_rx);
}
