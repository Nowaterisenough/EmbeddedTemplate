# 可选外部 STM32CubeF4 根目录；留空则使用仓库内 drivers/STM32CubeF4
set(HAL_EXTERNAL_PATH_F4 "C:/Users/NoWat/Workspace/3rd/stm32hal/STM32CubeF4" CACHE PATH "Path to external STM32CubeF4 root")
if(HAL_EXTERNAL_PATH_F4)
  set(STM32CUBEF4_ROOT "${HAL_EXTERNAL_PATH_F4}")
else()
  set(STM32CUBEF4_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/STM32CubeF4")
endif()

if(NOT EXISTS "${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h")
  message(FATAL_ERROR
    "未找到 STM32CubeF4 于: ${STM32CUBEF4_ROOT}\n"
    "请将 STM32CubeF4 放到 drivers/STM32CubeF4，或配置 -DHAL_EXTERNAL_PATH_F4=C:/path/to/STM32CubeF4"
  )
endif()

message(STATUS "STM32CubeF4: ${STM32CUBEF4_ROOT}")

# 供应商头（CMSIS + HAL）
add_library(cubef4 INTERFACE)
target_include_directories(cubef4 INTERFACE
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
  ${STM32CUBEF4_ROOT}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
  ${STM32CUBEF4_ROOT}/Drivers/CMSIS/Include
)

# HAL 源清单（可按需扩展）
set(CUBEF4_HAL_SOURCES
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
  ${STM32CUBEF4_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart_ex.c
  CACHE INTERNAL "HAL source files for STM32F4"
)