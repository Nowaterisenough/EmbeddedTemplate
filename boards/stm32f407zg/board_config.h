/**
 ******************************************************************************
 * @file    board_config.h
 * @brief   STM32F407ZG board configuration header
 ******************************************************************************
 */

#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* STM32 Family includes */
#include "stm32f4xx_hal.h"

/* Board specific defines */
#define BOARD_STM32F407ZG 1
#define BOARD_HAS_LED     1

/* LED 硬件映射表：定义逻辑 LED 到物理引脚的映射 */
#define BOARD_LED_MAP                                 \
    {                                                 \
        { GPIOD, GPIO_PIN_12 },     /* BOARD_LED_1 */ \
            { GPIOD, GPIO_PIN_13 }, /* BOARD_LED_2 */ \
            { GPIOD, GPIO_PIN_14 }, /* BOARD_LED_3 */ \
        {                                             \
            GPIOD, GPIO_PIN_15                        \
        } /* BOARD_LED_4 */                           \
    }

/* LED 极性：1=高电平点亮，0=低电平点亮 */
#define BOARD_LED_ACTIVE_HIGH 1

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_CONFIG_H */