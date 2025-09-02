# boards/stm32f407zg/CMakeLists.txt

# 依赖 drivers 层已定义的 HAL 源清单与供应商头目标
if(NOT DEFINED CUBEF4_HAL_SOURCES)
  message(FATAL_ERROR "CUBEF4_HAL_SOURCES 未定义，请先 add_subdirectory(drivers)")
endif()

set(BOARD_DIR ${CMAKE_CURRENT_LIST_DIR})

# 1) 创建真实 INTERFACE 目标（无 ::）
add_library(board_stm32f407zg INTERFACE)

# 2) 创建命名空间别名，供上层/应用链接使用
add_library(board::stm32f407zg ALIAS board_stm32f407zg)
set(MCU_BOARD_TARGET board::stm32f407zg PARENT_SCOPE)

# 板级与 HAL 源随 INTERFACE 传播
set(BOARD_SOURCES
  ${BOARD_DIR}/startup/startup_stm32f407xx.s
  ${BOARD_DIR}/config/stm32f4xx_it.c
  ${BOARD_DIR}/config/stm32f4xx_hal_msp.c
  ${BOARD_DIR}/config/system_stm32f4xx.c
  ${BOARD_DIR}/board.c
)

target_sources(board_stm32f407zg INTERFACE
  ${BOARD_SOURCES}
  ${CUBEF4_HAL_SOURCES}
)

# 头文件路径：板根与 config；工程公共头可按需添加
target_include_directories(board_stm32f407zg INTERFACE
  ${BOARD_DIR}
  ${BOARD_DIR}/config
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

# 编译选项与宏（随板）
target_compile_definitions(board_stm32f407zg INTERFACE
  USE_HAL_DRIVER
  STM32F407xx
)
target_compile_options(board_stm32f407zg INTERFACE
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -ffunction-sections -fdata-sections
)

# 供应商头（CMSIS/HAL）
target_link_libraries(board_stm32f407zg INTERFACE cubef4)

# 链接脚本（ARMClang 使用 .sct；如用 GCC，请补充 .ld 和 -T）
set(_SCT ${BOARD_DIR}/linker/stm32f407zgtx_flash.sct)
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  if(NOT EXISTS "${_SCT}")
    message(FATAL_ERROR "未找到 scatter 文件: ${_SCT}")
  endif()
  target_link_options(board_stm32f407zg INTERFACE
    --scatter=${_SCT}
    --info sizes --map --strict
  )
else()
  message(WARNING "当前非 ARMClang 工具链，请为本板提供 .ld 并在此添加 -T<ld>")
endif()