#include "stm32f4xx_hal.h"
#include "board.h"
#include "board_config.h"

static void SystemClock_Config(void);

void board_init(void)
{
    HAL_Init();
    SystemClock_Config();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

/* SystemClock 配置：HSE 8MHz + PLL -> SYSCLK 168MHz (最大性能)
 *
 * 时钟树：
 *   HSE        = 8 MHz (外部晶振)
 *   VCO input  = HSE/PLLM = 8/4 = 2 MHz
 *   VCO output = 2 * PLLN = 2 * 168 = 336 MHz
 *   SYSCLK     = VCO/PLLP = 336/2 = 168 MHz
 *   USB/SDIO   = VCO/PLLQ = 336/7 = 48 MHz (USB 需要)
 *   AHB        = SYSCLK = 168 MHz
 *   APB1       = AHB/4 = 42 MHz (最大 42 MHz)
 *   APB2       = AHB/2 = 84 MHz (最大 84 MHz)
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* 使能电源时钟 */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* 配置电压调节器输出电压 (Scale1 模式用于高性能) */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* 配置 HSE 和 PLL */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM            = 4;    /* VCO input = 8/4 = 2 MHz */
    RCC_OscInitStruct.PLL.PLLN            = 168;  /* VCO output = 2*168 = 336 MHz */
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2; /* SYSCLK = 336/2 = 168 MHz */
    RCC_OscInitStruct.PLL.PLLQ            = 7;    /* USB = 336/7 = 48 MHz */

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /* 配置系统时钟和总线分频 */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                   RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;   /* AHB = 168 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;     /* APB1 = 42 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;     /* APB2 = 84 MHz */

    /* Flash 延迟 5 等待周期 (168MHz @ 3.3V) */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
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

/* 错误信息存储（用于调试） */
static volatile board_error_info_t last_error = {0};

void board_error_handler(const char *file, uint32_t line, const char *func)
{
    /* 记录错误信息 */
    last_error.file = file;
    last_error.line = line;
    last_error.func = func;

    /* 在调试器中设置断点可查看 last_error */
    board_fatal_halt();
}

void board_fatal_halt(void)
{
    __disable_irq();
    while (1) {
        /* 可选：LED 闪烁表示错误状态 */
        #if BOARD_HAS_LED && defined(DEBUG)
        board_led_toggle(BOARD_LED_1);
        for (volatile uint32_t i = 0; i < 1000000; i++);
        #endif
    }
}