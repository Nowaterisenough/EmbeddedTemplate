/**
  ******************************************************************************
  * @file    stm32f4xx_hal_msp.c
  * @brief   HAL MSP module.
  ******************************************************************************
  */


#include "stm32f4xx_hal.h"

/**
  * @brief  Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}