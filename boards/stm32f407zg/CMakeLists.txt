# boards/stm32f407zg/CMakeLists.txt

# 依赖 drivers 层
if(NOT TARGET cubef4 OR NOT TARGET stm32f4_hal OR NOT TARGET hal_conf OR NOT TARGET hal_defs OR NOT TARGET hal_compile_opts)
  message(FATAL_ERROR "请先 add_subdirectory(drivers/STM32F4)")
endif()

set(BOARD_DIR ${CMAKE_CURRENT_LIST_DIR})

# 1) 创建真实 INTERFACE 目标（无 ::）
add_library(board_stm32f407zg INTERFACE)

# 2) 创建命名空间别名，供上层/应用链接使用
add_library(board::stm32f407zg ALIAS board_stm32f407zg)
set(MCU_BOARD_TARGET board::stm32f407zg PARENT_SCOPE)

# 根据编译器选择启动文件
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  set(STARTUP_FILE ${BOARD_DIR}/startup/startup_stm32f407xx.s)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set(STARTUP_FILE ${BOARD_DIR}/startup/startup_stm32f407xx_gcc.S)
else()
  message(FATAL_ERROR "不支持的编译器: ${CMAKE_C_COMPILER_ID}")
endif()

# 板级与 HAL 源随 INTERFACE 传播
set(BOARD_SOURCES
  ${STARTUP_FILE}
  ${BOARD_DIR}/config/stm32f4xx_it.c
  ${BOARD_DIR}/config/stm32f4xx_hal_msp.c
  ${BOARD_DIR}/config/system_stm32f4xx.c
  ${BOARD_DIR}/board.c
)

# 源文件随接口传播
target_sources(board_stm32f407zg INTERFACE ${BOARD_SOURCES})

# 头文件路径：板根与 config；工程公共头可按需添加
target_include_directories(board_stm32f407zg INTERFACE
  ${BOARD_DIR}
  ${BOARD_DIR}/config
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

# 编译选项与宏（随板）
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_definitions(board_stm32f407zg INTERFACE
    USE_HAL_DRIVER
    STM32F407xx
    USE_HAL_DRIVER_DEFAULT_TIMEOUT
    # 对于 GCC，禁用原子操作以避免内建函数问题
    # 或者在必要时可以定义替代版本
  )
else()
  target_compile_definitions(board_stm32f407zg INTERFACE
    USE_HAL_DRIVER
    STM32F407xx
  )
endif()
target_compile_options(board_stm32f407zg INTERFACE
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -march=armv7e-m
  -ffunction-sections -fdata-sections
)

# 将板级 config 目录注入 hal_conf，让 HAL 源能找到 stm32f4xx_hal_conf.h
target_include_directories(hal_conf INTERFACE
  ${BOARD_DIR}/config
)

# 把芯片宏与 USE_HAL_DRIVER 注入 hal_defs，让 stm32f4_hal 在编译时可见
target_compile_definitions(hal_defs INTERFACE
  USE_HAL_DRIVER
  STM32F407xx
)

# 将CPU相关编译选项注入 hal_compile_opts，让 stm32f4_hal 在编译时可见
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  target_compile_options(hal_compile_opts INTERFACE
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
  )
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(hal_compile_opts INTERFACE
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
  )
endif()

# 与供应商头/库链接
target_link_libraries(board_stm32f407zg INTERFACE stm32f4 cubef4)

# 链接脚本（ARMClang 使用 .sct；GCC 使用 .ld）
set(_SCT ${BOARD_DIR}/linker/stm32f407zgtx_flash.sct)
set(_LD ${BOARD_DIR}/linker/stm32f407zgtx_flash.ld)
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  if(NOT EXISTS "${_SCT}")
    message(FATAL_ERROR "未找到 scatter 文件: ${_SCT}")
  endif()
  target_link_options(board_stm32f407zg INTERFACE
    --scatter=${_SCT}
    --info sizes --map --strict
  )
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  if(NOT EXISTS "${_LD}")
    message(FATAL_ERROR "未找到 GNU LD 链接脚本: ${_LD}")
  endif()
  target_link_options(board_stm32f407zg INTERFACE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -march=armv7e-m
    -specs=nano.specs
    -T${_LD}
    -Wl,-Map=${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
    -Wl,--allow-multiple-definition
    -static
    -Wl,--start-group -lc -lm -Wl,--end-group
  )
else()
  message(WARNING "未知编译器 ${CMAKE_C_COMPILER_ID}，请手动配置链接选项")
endif()