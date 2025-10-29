/**
 * @file    uart_driver.c
 * @brief   UART驱动实现 - DMA + 空闲中断
 * @note    当前仅支持STM32H7系列，使用USART3
 */

#include "uart_driver.h"
#include "stm32h7xx_hal.h"

#define UART_INSTANCE USART3

/* ========================================================================
 * 私有变量
 * ======================================================================== */

static UART_HandleTypeDef huart;
static DMA_HandleTypeDef  hdma_rx;

static uint8_t            rx_buffer[UART_RX_BUFFER_SIZE];
static uart_rx_callback_t rx_callback = NULL;
static uint16_t           last_rx_pos = 0;  /* 上次DMA位置 */

/* ========================================================================
 * 私有函数
 * ======================================================================== */

/**
 * 处理接收到的数据
 */
static void process_received_data(void)
{
    if (!rx_callback) {
        return;
    }

    /* 获取当前DMA传输位置 */
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

/* ========================================================================
 * HAL层初始化
 * ======================================================================== */

/**
 * STM32H7 UART3初始化 (PB10=TX, PB11=RX)
 */
static bool hal_uart_init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能时钟 */
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* 配置GPIO: PB10=TX, PB11=RX */
    GPIO_InitStruct.Pin       = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 配置UART */
    huart.Instance          = USART3;
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

    /* 配置DMA: Stream1用于RX */
    hdma_rx.Instance                 = DMA1_Stream1;
    hdma_rx.Init.Request             = DMA_REQUEST_USART3_RX;
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

    /* 使能UART空闲中断 */
    __HAL_UART_ENABLE_IT(&huart, UART_IT_IDLE);

    /* 配置NVIC */
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);

    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

    return true;
}

/* ========================================================================
 * 公共API实现
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

    /* 启动DMA接收 */
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

void uart_idle_callback(void)
{
    /* 清除IDLE标志 */
    __HAL_UART_CLEAR_IDLEFLAG(&huart);

    /* 处理接收数据 */
    process_received_data();
}

/* ========================================================================
 * 中断服务函数
 * ======================================================================== */

void USART3_IRQHandler(void)
{
    /* 检查是否是空闲中断 */
    if (__HAL_UART_GET_FLAG(&huart, UART_FLAG_IDLE)) {
        uart_idle_callback();
    }

    HAL_UART_IRQHandler(&huart);
}

void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_rx);
}
