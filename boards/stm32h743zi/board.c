#include "board.h" // 统一接口原型

#include <stdint.h>

#include "board_config.h" // 板宏 + HAL 族头

static void MPU_Config(void);
static void SystemClock_Config(void);

void board_init(void)
{
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
void board_led_init(void)
{
    /* 已在 board_init 中完成，可留空 */
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStruct;

    /*开启LED相关的GPIO外设时钟*/
    BOARD_LED1_GPIO_CLK_ENABLE();
    BOARD_LED2_GPIO_CLK_ENABLE();
    BOARD_LED3_GPIO_CLK_ENABLE();

    /*选择要控制的GPIO引脚*/
    GPIO_InitStruct.Pin = BOARD_LED1_PIN;

    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

    /*设置引脚为上拉模式*/
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    /*设置引脚速率为高速 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
    HAL_GPIO_Init(BOARD_LED1_GPIO_PORT, &GPIO_InitStruct);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStruct.Pin = BOARD_LED2_PIN;
    HAL_GPIO_Init(BOARD_LED2_GPIO_PORT, &GPIO_InitStruct);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStruct.Pin = BOARD_LED3_PIN;
    HAL_GPIO_Init(BOARD_LED3_GPIO_PORT, &GPIO_InitStruct);

    board_led(BOARD_LED1_PIN, OFF); // 关闭LED1
    board_led(BOARD_LED2_PIN, OFF); // 关闭LED2
    board_led(BOARD_LED3_PIN, OFF); // 关闭LED3
}

void board_led(uint16_t led, uint8_t state)
{
    GPIO_TypeDef *led_port;

    switch (led) {

        case BOARD_LED1_PIN: led_port = BOARD_LED1_GPIO_PORT; break;
        case BOARD_LED2_PIN: led_port = BOARD_LED2_GPIO_PORT; break;
        case BOARD_LED3_PIN: led_port = BOARD_LED3_GPIO_PORT; break;
        default: return; // 无效的 LED 引脚
    }
    HAL_GPIO_WritePin(led_port, led, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
#endif

/* 你的原实现保持不变 -----------------------*/
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    HAL_StatusTypeDef ret;

    /* 电源与电压缩放 */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

    /* 如需 LSE（RTC），可在此启用
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    */

    /* 先提高 Flash 等待周期，避免切换到高频时过冲 */
    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_5);

    /* 启用外部无源晶振 HSE，并配置 PLL1 以得到 SYSCLK = 240 MHz */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;      /* 野火板为晶振，非 BYPASS */
    RCC_OscInitStruct.HSIState       = RCC_HSI_OFF;
    RCC_OscInitStruct.CSIState       = RCC_CSI_OFF;

    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;

    /* HSE = 25 MHz
       VCOin  = 25 / M = 5 MHz (范围 4~8 MHz，对应 VCI RANGE_2)
       VCOout = 5 * N = 480 MHz
       PLL1P  = 480 / 2 = 240 MHz -> SYSCLK
       PLL1Q  = 480 / 4 = 120 MHz -> 可用于 USB/SDMMC/SAI 等
       PLL1R  = 480 / 2 = 240 MHz -> 备用
    */
    RCC_OscInitStruct.PLL.PLLM      = 5;
    RCC_OscInitStruct.PLL.PLLN      = 96;
    RCC_OscInitStruct.PLL.PLLP      = 2;
    RCC_OscInitStruct.PLL.PLLQ      = 4;
    RCC_OscInitStruct.PLL.PLLR      = 2;
    RCC_OscInitStruct.PLL.PLLFRACN  = 0;

    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLRGE    = RCC_PLL1VCIRANGE_2; /* 4-8 MHz */

    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK)
    {
        while (1) { }
    }

    /* 选择 PLL1 为系统时钟，配置各总线分频 */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
                                     | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1
                                     | RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1;

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK; /* 240 MHz */
    RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;         /* 240 MHz */
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;           /* 120 MHz */
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;           /* 60 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;           /* 60 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;           /* 60 MHz */
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;           /* 60 MHz */

    /* HAL v1.11.5 更保守，240 MHz 建议使用 FLASH_LATENCY_4（若经高温/低压验证稳定，可尝试 3） */
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
    if (ret != HAL_OK)
    {
        while (1) { }
    }

    /* 如需 USB 48 MHz，可后续配置 PLL3 或者从 120 MHz 进一步分频生成，视工程而定 */
}

void board_fatal_halt(void)
{
    __disable_irq();
    while (1) {
    }
}