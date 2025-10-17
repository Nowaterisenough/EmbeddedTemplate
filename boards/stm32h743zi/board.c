#include "board.h" // 统一接口原型

#include <stdint.h>

#include "board_config.h" // 板宏 + HAL 族头

static void MPU_Config(void);
static void SystemClock_Config(void);

void board_init(void)
{
    /* Enable I-Cache---------------------------------------------------------*/
    SCB_EnableICache();

    /* Enable D-Cache---------------------------------------------------------*/
    SCB_EnableDCache();

    HAL_Init();
    SystemClock_Config();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
}

/* 简单封装，应用层不直接调用 HAL */
void board_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

uint32_t board_millis(void)
{
    return HAL_GetTick();
}

#if BOARD_HAS_LED
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
        .Pull  = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_HIGH
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
#endif

/* 你的原实现保持不变 -----------------------*/
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
    HAL_StatusTypeDef  ret;

    /* 使能供电配置更新 */
    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

    /* 电源与电压缩放 */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }

    /* 如需 LSE（RTC），可在此启用
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    */

    /* 先设置最高 Flash 等待周期，避免切换到高频时过冲 */
    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_7);
    if (__HAL_FLASH_GET_LATENCY() != FLASH_LATENCY_7) {
        /* Flash latency setting failed */
        while (1) {
        }
    }

    /* 启用外部无源晶振 HSE，并配置 PLL1 以得到 SYSCLK = 240 MHz */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON; /* 野火板为晶振，非 BYPASS */
    RCC_OscInitStruct.HSIState       = RCC_HSI_OFF;
    RCC_OscInitStruct.CSIState       = RCC_CSI_OFF;

    RCC_OscInitStruct.PLL.PLLState  = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    /* HSE = 25 MHz
       VCOin  = 25 / M = 5 MHz (范围 4~8 MHz，对应 VCI RANGE_2)
       VCOout = 5 * N = 800 MHz
       PLL1P  = 800 / 2 = 400 MHz -> SYSCLK
       PLL1Q  = 800 / 4 = 200 MHz -> 可用于 USB/SDMMC/SAI 等
       PLL1R  = 800 / 2 = 400 MHz -> 备用
    */
    RCC_OscInitStruct.PLL.PLLM     = 5;
    RCC_OscInitStruct.PLL.PLLN     = 160;
    RCC_OscInitStruct.PLL.PLLP     = 2;
    RCC_OscInitStruct.PLL.PLLQ     = 4;
    RCC_OscInitStruct.PLL.PLLR     = 2;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;

    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLRGE    = RCC_PLL1VCIRANGE_2; /* 4-8 MHz */

    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK) {
        /* HAL_RCC_OscConfig failed */
        while (1) {
        }
    }

    /* 选择 PLL1 为系统时钟，配置各总线分频 */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 |
                                   RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1);

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK; /* 400 MHz */
    RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;         /* 400 MHz */
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;           /* 200 MHz */
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;           /* 100 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;           /* 100 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;           /* 100 MHz */
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;           /* 100 MHz */

    /* 先尝试更保守的Flash延迟设置 */
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
    if (ret != HAL_OK) {

        /* HAL_RCC_ClockConfig failed with all latencies */
        while (1) {
        }
    }

    /* 如需 USB 48 MHz，可后续配置 PLL3 或者从 120 MHz 进一步分频生成，视工程而定 */
}

void board_fatal_halt(void)
{
    __disable_irq();
    while (1) {
    }
}