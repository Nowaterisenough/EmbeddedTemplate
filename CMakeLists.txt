cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0123 NEW)

# 必须在project()之前设置系统和处理器
set(CMAKE_SYSTEM_NAME Generic)

project(EmbeddedTemplate C ASM)  # 启用 C 与 ASM

# 根据板子设置CPU相关编译选项（在包含3rd库之前）
if(NOT DEFINED BOARD)
  set(BOARD "stm32h743zi" CACHE STRING "Target board")
endif()

# 为第三方库设置CPU相关编译选项
if(BOARD STREQUAL "stm32h743zi")
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
    set(BOARD_COMPILE_OPTIONS "-mcpu=cortex-m7;-mfpu=fpv5-d16;-mfloat-abi=hard")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(BOARD_COMPILE_OPTIONS "-mcpu=cortex-m7;-mfpu=fpv5-d16;-mfloat-abi=hard")
  endif()
elseif(BOARD STREQUAL "stm32f407zg")
  if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
    set(BOARD_COMPILE_OPTIONS "-mcpu=cortex-m4;-mfpu=fpv4-sp-d16;-mfloat-abi=hard")
  elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(BOARD_COMPILE_OPTIONS "-mcpu=cortex-m4;-mfpu=fpv4-sp-d16;-mfloat-abi=hard")
  endif()
endif()

# 添加子目录，使用模块化构建
add_subdirectory(3rd)
add_subdirectory(drivers) 
add_subdirectory(boards)
add_subdirectory(app)

