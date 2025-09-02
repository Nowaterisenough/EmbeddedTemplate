#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "stm32h7xx.h"

#define BOARD_HAS_LED                1

#define BOARD_LED1_PIN               GPIO_PIN_10
#define BOARD_LED1_GPIO_PORT         GPIOH
#define BOARD_LED1_GPIO_CLK_ENABLE() __GPIOH_CLK_ENABLE()

#define BOARD_LED2_PIN               GPIO_PIN_11
#define BOARD_LED2_GPIO_PORT         GPIOH
#define BOARD_LED2_GPIO_CLK_ENABLE() __GPIOH_CLK_ENABLE()

#define BOARD_LED3_PIN               GPIO_PIN_12
#define BOARD_LED3_GPIO_PORT         GPIOH
#define BOARD_LED3_GPIO_CLK_ENABLE() __GPIOH_CLK_ENABLE()

#define ON                           GPIO_PIN_RESET
#define OFF                          GPIO_PIN_SET

#endif
