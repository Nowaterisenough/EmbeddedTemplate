# Toolchain file for ARM Compiler 6 (armclang)

# 设置CMake策略CMP0123为NEW，避免自动添加CPU/架构标志
if(POLICY CMP0123)
    cmake_policy(SET CMP0123 NEW)
endif()

set(CMAKE_SYSTEM_NAME Generic)

# 必须在project()之前设置这些变量
set(CMAKE_SYSTEM_PROCESSOR cortex-m7)

# ARM Compiler 6 installation path - read from environment (set by CMakePresets.json)
set(ARM_TOOLCHAIN_PATH "$ENV{ARM_CLANG_PATH}")

# Fallback to default path if environment variable is not set
if(NOT ARM_TOOLCHAIN_PATH OR ARM_TOOLCHAIN_PATH STREQUAL "")
    set(ARM_TOOLCHAIN_PATH "C:/Users/NoWat/Workspace/Toolchains/ArmCompiler6.22/bin")
    message(WARNING "ARM_CLANG_PATH not set, using default: ${ARM_TOOLCHAIN_PATH}")
endif()

# Specify the cross compiler - use environment variables if available, otherwise construct paths
if(DEFINED ENV{ARMCLANG_EXE})
    set(CMAKE_C_COMPILER "$ENV{ARMCLANG_EXE}")
    set(CMAKE_CXX_COMPILER "$ENV{ARMCLANG_EXE}")
    set(CMAKE_ASM_COMPILER "$ENV{ARMCLANG_EXE}")
else()
    set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_PATH}/armclang.exe")
    set(CMAKE_CXX_COMPILER "${ARM_TOOLCHAIN_PATH}/armclang.exe")
    set(CMAKE_ASM_COMPILER "${ARM_TOOLCHAIN_PATH}/armclang.exe")
endif()

# Specify additional tools
if(DEFINED ENV{FROMELF_EXE})
    set(CMAKE_OBJCOPY "$ENV{FROMELF_EXE}")
    set(CMAKE_OBJDUMP "$ENV{FROMELF_EXE}")
    set(CMAKE_SIZE_UTIL "$ENV{FROMELF_EXE}")
else()
    set(CMAKE_OBJCOPY "${ARM_TOOLCHAIN_PATH}/fromelf.exe")
    set(CMAKE_OBJDUMP "${ARM_TOOLCHAIN_PATH}/fromelf.exe")
    set(CMAKE_SIZE_UTIL "${ARM_TOOLCHAIN_PATH}/fromelf.exe")
endif()

if(DEFINED ENV{ARMAR_EXE})
    set(CMAKE_AR "$ENV{ARMAR_EXE}")
    set(CMAKE_RANLIB "$ENV{ARMAR_EXE}")
else()
    set(CMAKE_AR "${ARM_TOOLCHAIN_PATH}/armar.exe")
    set(CMAKE_RANLIB "${ARM_TOOLCHAIN_PATH}/armar.exe")
endif()

if(DEFINED ENV{ARMLINK_EXE})
    set(CMAKE_LINKER "$ENV{ARMLINK_EXE}")
else()
    set(CMAKE_LINKER "${ARM_TOOLCHAIN_PATH}/armlink.exe")
endif()

# 强制设置编译器ID
set(CMAKE_C_COMPILER_ID ARMClang)
set(CMAKE_CXX_COMPILER_ID ARMClang)
set(CMAKE_ASM_COMPILER_ID ARMClang)

# 强制设置编译器版本（避免检测）
set(CMAKE_C_COMPILER_VERSION 6.22)
set(CMAKE_CXX_COMPILER_VERSION 6.22)
set(CMAKE_ASM_COMPILER_VERSION 6.22)

# 禁用编译器检查
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_ASM_COMPILER_FORCED TRUE)

# 告诉CMake编译器可以工作
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_ASM_COMPILER_WORKS TRUE)

# 验证工具链文件存在
if(NOT EXISTS "${CMAKE_C_COMPILER}")
    message(FATAL_ERROR "ARM Compiler not found at: ${CMAKE_C_COMPILER}")
endif()

if(NOT EXISTS "${CMAKE_OBJCOPY}")
    message(FATAL_ERROR "fromelf not found at: ${CMAKE_OBJCOPY}")
endif()

if(NOT EXISTS "${CMAKE_LINKER}")
    message(FATAL_ERROR "armlink not found at: ${CMAKE_LINKER}")
endif()

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
message(STATUS "  Toolchain Path: ${ARM_TOOLCHAIN_PATH}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  ASM Compiler: ${CMAKE_ASM_COMPILER}")
message(STATUS "  Linker: ${CMAKE_LINKER}")
message(STATUS "  Object Copy: ${CMAKE_OBJCOPY}")
message(STATUS "  Size Utility: ${CMAKE_SIZE_UTIL}")
message(STATUS "  Archiver: ${CMAKE_AR}")