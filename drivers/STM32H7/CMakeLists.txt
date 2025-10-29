# drivers/STM32H7/CMakeLists.txt

# 可选外部 STM32CubeH7 根目录；留空则使用仓库内 drivers/STM32H7/STM32CubeH7
set(HAL_EXTERNAL_PATH "" CACHE PATH "Path to external STM32CubeH7 root")
if(HAL_EXTERNAL_PATH)
  set(STM32CUBEH7_ROOT "${HAL_EXTERNAL_PATH}")
else()
  set(STM32CUBEH7_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/STM32CubeH7")
endif()

if(NOT EXISTS "${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal.h")
  message(FATAL_ERROR
    "未找到 STM32CubeH7 于: ${STM32CUBEH7_ROOT}\n"
    "请将 STM32CubeH7 放到 drivers/STM32H7/STM32CubeH7，或配置 -DHAL_EXTERNAL_PATH=C:/path/to/STM32CubeH7"
  )
endif()

message(STATUS "STM32CubeH7: ${STM32CUBEH7_ROOT}")

# 供应商头（CMSIS + HAL）
add_library(cubeh7 INTERFACE)
target_include_directories(cubeh7 INTERFACE
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
  ${STM32CUBEH7_ROOT}/Drivers/CMSIS/Include
  ${STM32CUBEH7_ROOT}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
)

# HAL 源清单（按需增删）
set(CUBEH7_HAL_SOURCES
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma2d.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_lptim.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hrtim.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_iwdg.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_wwdg.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_usart.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_smbus.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_swpmi.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_irda.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_smartcard.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_smartcard_ex.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2s.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2s_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sai.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sai_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spdifrx.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dfsdm.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_jpeg.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dac.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dac_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_comp.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_opamp.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_opamp_ex.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sd.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sd_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mmc.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mmc_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_qspi.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_nor.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_nand.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sram.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sdram.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_eth.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_eth_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_fdcan.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cec.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdios.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hcd.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_ltdc.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_ltdc_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dcmi.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cryp.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cryp_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hash.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hash_ex.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rng.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rtc.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rtc_ex.c

  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_crc.c
  ${STM32CUBEH7_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_crc_ex.c

  CACHE INTERNAL "HAL source files for STM32H7"
)

# HAL 静态库
add_library(stm32h7_hal STATIC ${CUBEH7_HAL_SOURCES})
target_link_libraries(stm32h7_hal PUBLIC cubeh7)

# 默认宏（具体芯片在板级注入）
target_compile_definitions(stm32h7_hal PUBLIC USE_HAL_DRIVER)

# 关键：为HAL库添加与应用代码一致的编译选项，避免relocation错误
# 这些选项必须与最终链接的应用代码保持一致
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(stm32h7_hal PRIVATE
    -ffunction-sections
    -fdata-sections
  )
endif()

# 提供"HAL 配置头路径"的接口容器，供板级注入 config 目录
add_library(hal_conf INTERFACE)
# 提供"编译宏"的接口容器，供板级注入芯片宏
add_library(hal_defs INTERFACE)
# 提供"编译选项"的接口容器，供板级注入CPU相关编译选项
add_library(hal_compile_opts INTERFACE)

# 使 stm32h7_hal 在编译自身源码时消费 hal_conf, hal_defs 和 hal_compile_opts
target_link_libraries(stm32h7_hal PUBLIC hal_conf hal_defs hal_compile_opts)

# 组合接口目标
add_library(stm32h7 INTERFACE)
target_link_libraries(stm32h7 INTERFACE stm32h7_hal cubeh7)

# 导出变量到父作用域
set(CUBEH7_HAL_SOURCES ${CUBEH7_HAL_SOURCES} PARENT_SCOPE)