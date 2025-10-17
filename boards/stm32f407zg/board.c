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

/* LED 硬件映射表 */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    void        (*clk_enable)(void);
} led_map_t;

static const led_map_t led_map[] = BOARD_LED_MAP;
static uint8_t led_state[BOARD_LED_MAX] = {0};

void board_led_init(void)
{
    GPIO_InitTypeDef gpio_init = {
        .Mode  = GPIO_MODE_OUTPUT_PP,
        .Pull  = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

    for (size_t i = 0; i < BOARD_LED_MAX; i++) {
        if (led_map[i].port == NULL) continue;

        /* 使能时钟 */
        if (led_map[i].clk_enable) {
            led_map[i].clk_enable();
        }

        /* 初始化 GPIO */
        gpio_init.Pin = led_map[i].pin;
        HAL_GPIO_Init(led_map[i].port, &gpio_init);

        /* 默认关闭所有 LED */
        board_led_set((board_led_id_t)i, LED_OFF);
    }
}

void board_led_set(board_led_id_t led, led_state_t state)
{
    if (led >= BOARD_LED_MAX || led_map[led].port == NULL) {
        return;
    }

    led_state[led] = state;

    GPIO_PinState pin_state;
    #if BOARD_LED_ACTIVE_HIGH
        pin_state = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    #else
        pin_state = state ? GPIO_PIN_RESET : GPIO_PIN_SET;
    #endif

    HAL_GPIO_WritePin(led_map[led].port, led_map[led].pin, pin_state);
}

void board_led_toggle(board_led_id_t led)
{
    if (led >= BOARD_LED_MAX) {
        return;
    }

    board_led_set(led, led_state[led] ? LED_OFF : LED_ON);
}

void board_fatal_halt(void)
{
    __disable_irq();
    while (1) {
        /* Halt here */
    }
}