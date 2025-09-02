# Toolchain file for ARM GCC (arm-none-eabi-gcc)

# 设置CMake策略CMP0123为NEW，避免自动添加CPU/架构标志
if(POLICY CMP0123)
    cmake_policy(SET CMP0123 NEW)
endif()

set(CMAKE_SYSTEM_NAME Generic)

# 必须在project()之前设置这些变量
set(CMAKE_SYSTEM_PROCESSOR cortex-m7)

# 可选 PATH 提示：通过 VS Code settings 注入 ARM_GCC_PATH（cmake.configureSettings 或 cmake.environment）
set(ARM_GCC_PATH "${ARM_GCC_PATH}")
if(NOT ARM_GCC_PATH AND DEFINED ENV{ARM_GCC_PATH})
    set(ARM_GCC_PATH "$ENV{ARM_GCC_PATH}")
endif()
if(ARM_GCC_PATH)
    list(APPEND CMAKE_PROGRAM_PATH "${ARM_GCC_PATH}")
endif()

# 指定交叉编译器（采用标准可执行名，依赖 PATH 或 CMAKE_PROGRAM_PATH）
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# 其他工具（采用标准可执行名）
set(CMAKE_OBJCOPY   arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP   arm-none-eabi-objdump)
set(CMAKE_SIZE_UTIL arm-none-eabi-size)
set(CMAKE_AR        arm-none-eabi-ar)
set(CMAKE_RANLIB    arm-none-eabi-ranlib)
set(CMAKE_LINKER    arm-none-eabi-ld)

# 保持默认检测流程，不再强制伪造编译器 ID/版本/可用性

# 不做存在性检查，默认工具可用

# 设置搜索路径
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 禁用共享库
set(BUILD_SHARED_LIBS OFF)

# 设置可执行文件后缀
set(CMAKE_EXECUTABLE_SUFFIX_C .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# 设置对象文件扩展名
set(CMAKE_C_OUTPUT_EXTENSION .o)
set(CMAKE_CXX_OUTPUT_EXTENSION .o)
set(CMAKE_ASM_OUTPUT_EXTENSION .o)

# 设置默认编译标志
set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb")
set(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb")
set(CMAKE_ASM_FLAGS_INIT "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb")

# 统一的编译选项：O0 优化 + 调试信息（仅影响 ELF，不影响 BIN/HEX）
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -O0 -g -gdwarf-4")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -O0 -g -gdwarf-4")
set(CMAKE_ASM_FLAGS_INIT "${CMAKE_ASM_FLAGS_INIT} -g")

# 设置链接器标志
set(CMAKE_EXE_LINKER_FLAGS_INIT "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--gc-sections")

# 设置标准库
set(CMAKE_C_STANDARD_LIBRARIES "-lc -lm -lnosys")
set(CMAKE_CXX_STANDARD_LIBRARIES "-lstdc++ -lc -lm -lnosys")

# 调试信息输出
message(STATUS "ARM GCC toolchain configured:")
message(STATUS "  System Name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  System Architecture: ${CMAKE_SYSTEM_ARCH}")
message(STATUS "  Path Hint: ${ARM_GCC_PATH}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  ASM Compiler: ${CMAKE_ASM_COMPILER}")
message(STATUS "  Linker: ${CMAKE_LINKER}")
message(STATUS "  Object Copy: ${CMAKE_OBJCOPY}")
message(STATUS "  Size Utility: ${CMAKE_SIZE_UTIL}")
message(STATUS "  Archiver: ${CMAKE_AR}")