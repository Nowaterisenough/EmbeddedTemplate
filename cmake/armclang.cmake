# Toolchain file for ARM Compiler 6 (armclang)

# 设置CMake策略CMP0123为NEW，避免自动添加CPU/架构标志
if(POLICY CMP0123)
    cmake_policy(SET CMP0123 NEW)
endif()

set(CMAKE_SYSTEM_NAME Generic)

# 必须在project()之前设置这些变量
set(CMAKE_SYSTEM_PROCESSOR cortex-m7)

# 可选 PATH 提示：通过 VS Code settings 注入 ARM_CLANG_PATH（cmake.configureSettings 或 cmake.environment）
set(ARM_CLANG_PATH "${ARM_CLANG_PATH}")
if(NOT ARM_CLANG_PATH AND DEFINED ENV{ARM_CLANG_PATH})
    set(ARM_CLANG_PATH "$ENV{ARM_CLANG_PATH}")
endif()
if(ARM_CLANG_PATH)
    list(APPEND CMAKE_PROGRAM_PATH "${ARM_CLANG_PATH}")
endif()

# 使用标准可执行名；如需自定义，可在 VS Code settings 覆盖这些变量
set(CMAKE_C_COMPILER   armclang)
set(CMAKE_CXX_COMPILER armclang)
set(CMAKE_ASM_COMPILER armclang)

# Specify additional tools
set(CMAKE_OBJCOPY  fromelf)
set(CMAKE_OBJDUMP  fromelf)
set(CMAKE_SIZE_UTIL fromelf)

set(CMAKE_AR     armar)
set(CMAKE_RANLIB armar)

set(CMAKE_LINKER armlink)

# 让 CMake 自行检测编译器和版本，不再强制伪造

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

# 设置默认编译标志（移除-march参数，为ASM添加-masm=auto并抑制警告）
set(CMAKE_C_FLAGS_INIT "--target=arm-arm-none-eabi -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb")
set(CMAKE_CXX_FLAGS_INIT "--target=arm-arm-none-eabi -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb")
set(CMAKE_ASM_FLAGS_INIT "--target=arm-arm-none-eabi -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -masm=auto -Wno-deprecated")

# 统一的编译选项：O0 优化 + 调试信息（仅影响 ELF，不影响 BIN/HEX）
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -O0 -g -gdwarf-4")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -O0 -g -gdwarf-4")
set(CMAKE_ASM_FLAGS_INIT "${CMAKE_ASM_FLAGS_INIT} -g")

# 设置链接器标志
set(CMAKE_EXE_LINKER_FLAGS_INIT "")

# 设置标准库为空（嵌入式系统）
set(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_CXX_STANDARD_LIBRARIES "")

# 自定义链接命令
set(CMAKE_C_LINK_EXECUTABLE 
    "<CMAKE_LINKER> <OBJECTS> <LINK_OPTIONS> <LINK_LIBRARIES> --output=<TARGET>")
set(CMAKE_CXX_LINK_EXECUTABLE 
    "<CMAKE_LINKER> <OBJECTS> <LINK_OPTIONS> <LINK_LIBRARIES> --output=<TARGET>")

# 调试信息输出
message(STATUS "ARM Compiler 6.22 toolchain configured:")
message(STATUS "  System Name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  System Architecture: ${CMAKE_SYSTEM_ARCH}")
message(STATUS "  Path Hint: ${ARM_CLANG_PATH}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  ASM Compiler: ${CMAKE_ASM_COMPILER}")
message(STATUS "  Linker: ${CMAKE_LINKER}")
message(STATUS "  Object Copy: ${CMAKE_OBJCOPY}")
message(STATUS "  Size Utility: ${CMAKE_SIZE_UTIL}")
message(STATUS "  Archiver: ${CMAKE_AR}")