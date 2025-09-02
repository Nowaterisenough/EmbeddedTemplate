/**
  ******************************************************************************
  * @file    board_config.h
  * @brief   STM32F407ZG board configuration header
  ******************************************************************************
  */

#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Board specific defines */
#define BOARD_STM32F407ZG   1
#define BOARD_HAS_LED       1

/* LED state defines */
#define ON                  1
#define OFF                 0

/* LED pin defines */
#define BOARD_LED1_PIN      1
#define BOARD_LED2_PIN      2
#define BOARD_LED3_PIN      3
#define BOARD_LED4_PIN      4

/* STM32 Family includes */
#include "stm32f4xx_hal.h"
#include "main.h"

/* Board specific GPIO definitions */
#define LED1_PIN                    GPIO_PIN_12
#define LED1_GPIO_PORT              GPIOD
#define LED1_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()

#define LED2_PIN                    GPIO_PIN_13
#define LED2_GPIO_PORT              GPIOD
#define LED2_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()

#define LED3_PIN                    GPIO_PIN_14
#define LED3_GPIO_PORT              GPIOD
#define LED3_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()

#define LED4_PIN                    GPIO_PIN_15
#define LED4_GPIO_PORT              GPIOD
#define LED4_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_CONFIG_H */