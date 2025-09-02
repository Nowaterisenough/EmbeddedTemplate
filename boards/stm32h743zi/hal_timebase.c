#include "stm32h7xx_hal.h"

/* Private variables */
uint32_t uwTickPrio = 0U;

/**
 * @brief  Initialize the tick
 * @param  TickPriority: Tick interrupt priority
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    /* Use SysTick as time base source and configure 1ms tick */
    if (HAL_SYSTICK_Config(SystemCoreClock / 1000U) > 0U) {
        return HAL_ERROR;
    }

    /* Configure the SysTick IRQ priority */
    if (TickPriority < (1UL << __NVIC_PRIO_BITS)) {
        HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
        uwTickPrio = TickPriority;
    } else {
        return HAL_ERROR;
    }

    /* Return function status */
    return HAL_OK;
}