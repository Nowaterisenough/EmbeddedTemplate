# boards/stm32h743zi/CMakeLists.txt

# 依赖 drivers 层
if(NOT TARGET cubeh7 OR NOT TARGET stm32h7_hal OR NOT TARGET hal_conf OR NOT TARGET hal_defs OR NOT TARGET hal_compile_opts)
  message(FATAL_ERROR "请先 add_subdirectory(drivers/STM32H7)")
endif()

set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
set(CMAKE_EXE_LINKER_FLAGS_INIT "--cpu=cortex-m7")
set(BOARD_DIR ${CMAKE_CURRENT_LIST_DIR})

# INTERFACE 目标与别名
add_library(board_stm32h743zi INTERFACE)
add_library(board::stm32h743zi ALIAS board_stm32h743zi)
set(MCU_BOARD_TARGET board::stm32h743zi PARENT_SCOPE)

# 板级源文件
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  set(STARTUP_FILE ${BOARD_DIR}/startup/startup_stm32h743xx.s)
else()
  set(STARTUP_FILE ${BOARD_DIR}/startup/startup_stm32h743xx.s)
endif()

set(BOARD_SOURCES
  ${STARTUP_FILE}
  ${BOARD_DIR}/config/stm32h7xx_it.c
  ${BOARD_DIR}/config/system_stm32h7xx.c
  ${BOARD_DIR}/board.c
)

# 头文件路径（优先板级 config）
target_include_directories(board_stm32h743zi INTERFACE
  ${BOARD_DIR}
  ${BOARD_DIR}/config
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

# 源文件随接口传播
target_sources(board_stm32h743zi INTERFACE ${BOARD_SOURCES})

# 将板级 config 目录注入 hal_conf，让 HAL 源能找到 stm32h7xx_hal_conf.h
target_include_directories(hal_conf INTERFACE
  ${BOARD_DIR}/config
)

# 把芯片宏与 USE_HAL_DRIVER 注入 hal_defs，让 stm32h7_hal 在编译时可见
target_compile_definitions(hal_defs INTERFACE
  USE_HAL_DRIVER
  STM32H743xx
)

# 将CPU相关编译选项注入 hal_compile_opts，让 stm32h7_hal 在编译时可见
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  target_compile_options(hal_compile_opts INTERFACE
    -mcpu=cortex-m7
    -mfpu=fpv5-d16
    -mfloat-abi=hard
  )
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(hal_compile_opts INTERFACE
    -mcpu=cortex-m7
    -mfpu=fpv5-d16
    -mfloat-abi=hard
  )
endif()

# 同时也让 board 目标本身带上这些宏（便于下游模块使用）
target_compile_definitions(board_stm32h743zi INTERFACE
  USE_HAL_DRIVER
  STM32H743xx
)

# 与供应商头/库链接
target_link_libraries(board_stm32h743zi INTERFACE stm32h7 cubeh7)

# 工具链与选项
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  target_compile_options(board_stm32h743zi INTERFACE
    -mcpu=cortex-m7
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -O0
  )
  # 链接脚本（ARMClang .sct）
  set(_SCT ${BOARD_DIR}/linker/stm32h743zitx_flash.sct)
  if(NOT EXISTS "${_SCT}")
    message(FATAL_ERROR "未找到 scatter 文件: ${_SCT}")
  endif()
  target_link_options(board_stm32h743zi INTERFACE
    --cpu=Cortex-M7.fp.dp
    --strict
    --scatter=${_SCT}
    --info sizes
    --map
    --entry=Reset_Handler
  )
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(board_stm32h743zi INTERFACE
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -ffunction-sections
    -fdata-sections
    -O0
  )
else()
  message(WARNING "未为当前工具链设置链接脚本，请补充对应配置。")
endif()