#include "stm32f4xx_hal.h"
#include "board.h"
#include "board_config.h"

static void SystemClock_Config(void);

void board_early_init(void)
{
    /* F4 无需特别的早期操作，可留空 */
}

void board_init(void)
{
    HAL_Init();
    SystemClock_Config();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

/* 最小 SystemClock：HSI 16MHz 作为 SYSCLK（先保证可用，后续你可改为 HSE+PLL） */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_OFF;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        while (1) { }
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                       RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        while (1) { }
    }
}

void board_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

uint32_t board_millis(void)
{
    return HAL_GetTick();
}

void board_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIO clocks for LEDs */
    LED1_GPIO_CLK_ENABLE();
    LED2_GPIO_CLK_ENABLE();
    LED3_GPIO_CLK_ENABLE();
    LED4_GPIO_CLK_ENABLE();

    /* Configure GPIO pins for LEDs */
    GPIO_InitStruct.Pin = LED1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED2_PIN;
    HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED3_PIN;
    HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED4_PIN;
    HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);
}

void board_led(uint16_t led, uint8_t state)
{
    GPIO_PinState pin_state = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    
    switch(led) {
        case BOARD_LED1_PIN:
            HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, pin_state);
            break;
        case BOARD_LED2_PIN:
            HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, pin_state);
            break;
        case BOARD_LED3_PIN:
            HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, pin_state);
            break;
        case BOARD_LED4_PIN:
            HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, pin_state);
            break;
        default:
            break;
    }
}

void board_fatal_halt(void)
{
    __disable_irq();
    while (1) {
        /* Halt here */
    }
}