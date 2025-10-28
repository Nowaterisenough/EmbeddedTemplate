#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "stm32h7xx.h"

#define BOARD_HAS_LED 1

/* LED 硬件映射表：定义逻辑 LED 到物理引脚的映射 */
#define BOARD_LED_MAP                                     \
    {                                                     \
        { GPIOH, GPIO_PIN_10 }, /* BOARD_LED_1 */         \
        { GPIOH, GPIO_PIN_11 }, /* BOARD_LED_2 */         \
        { GPIOH, GPIO_PIN_12 }, /* BOARD_LED_3 */         \
        { NULL,  0           }  /* BOARD_LED_4: 未使用 */ \
}

/* LED 极性：1=高电平点亮，0=低电平点亮 */
#define BOARD_LED_ACTIVE_HIGH 0

#endif
