# ARM Cortex-M GCC Toolchain (arm-none-eabi-gcc)
# 支持: Cortex-M0/M0+/M3/M4/M7/M23/M33/M55/M85
# 目标: 裸机嵌入式系统 (无操作系统)

# 设置目标系统信息（必须在其他设置之前）
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# 设置CMake策略CMP0123为NEW，避免自动添加CPU/架构标志
if(POLICY CMP0123)
    cmake_policy(SET CMP0123 NEW)
endif()

# 跳过编译器测试（交叉编译环境）
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_ASM_COMPILER_WORKS TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 可选 PATH 提示：通过 VS Code settings 注入 ARM_NONE_EABI_GCC_PATH
set(ARM_NONE_EABI_GCC_PATH "${ARM_NONE_EABI_GCC_PATH}")
if(NOT ARM_NONE_EABI_GCC_PATH AND DEFINED ENV{ARM_NONE_EABI_GCC_PATH})
    set(ARM_NONE_EABI_GCC_PATH "$ENV{ARM_NONE_EABI_GCC_PATH}")
endif()
# 向后兼容旧变量名
if(NOT ARM_NONE_EABI_GCC_PATH AND ARM_GCC_PATH)
    set(ARM_NONE_EABI_GCC_PATH "${ARM_GCC_PATH}")
endif()
if(NOT ARM_NONE_EABI_GCC_PATH AND DEFINED ENV{ARM_GCC_PATH})
    set(ARM_NONE_EABI_GCC_PATH "$ENV{ARM_GCC_PATH}")
endif()

if(ARM_NONE_EABI_GCC_PATH)
    set(ENV{PATH} "${ARM_NONE_EABI_GCC_PATH};$ENV{PATH}")
    list(APPEND CMAKE_PROGRAM_PATH "${ARM_NONE_EABI_GCC_PATH}")
    message(STATUS "  Added ARM_NONE_EABI_GCC_PATH to PATH: ${ARM_NONE_EABI_GCC_PATH}")
endif()

# 设置编译器和工具
if(ARM_NONE_EABI_GCC_PATH)
    if(WIN32)
        file(TO_CMAKE_PATH "${ARM_NONE_EABI_GCC_PATH}" ARM_NONE_EABI_GCC_PATH_NORM)
        set(CMAKE_C_COMPILER   "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-gcc.exe")
        set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-g++.exe")
        set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-gcc.exe")
        set(CMAKE_OBJCOPY      "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-objcopy.exe")
        set(CMAKE_OBJDUMP      "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-objdump.exe")
        set(CMAKE_SIZE_UTIL    "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-size.exe")
        set(CMAKE_AR           "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-ar.exe")
        set(CMAKE_RANLIB       "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-ranlib.exe")
        set(CMAKE_LINKER       "${ARM_NONE_EABI_GCC_PATH_NORM}/arm-none-eabi-ld.exe")
    else()
        set(CMAKE_C_COMPILER   "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-gcc")
        set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-g++")
        set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-gcc")
        set(CMAKE_OBJCOPY      "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-objcopy")
        set(CMAKE_OBJDUMP      "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-objdump")
        set(CMAKE_SIZE_UTIL    "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-size")
        set(CMAKE_AR           "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-ar")
        set(CMAKE_RANLIB       "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-ranlib")
        set(CMAKE_LINKER       "${ARM_NONE_EABI_GCC_PATH}/arm-none-eabi-ld")
    endif()
else()
    set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
    set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
    set(CMAKE_OBJCOPY      arm-none-eabi-objcopy)
    set(CMAKE_OBJDUMP      arm-none-eabi-objdump)
    set(CMAKE_SIZE_UTIL    arm-none-eabi-size)
    set(CMAKE_AR           arm-none-eabi-ar)
    set(CMAKE_RANLIB       arm-none-eabi-ranlib)
    set(CMAKE_LINKER       arm-none-eabi-ld)
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

# 设置基础编译标志（MCU特定选项由board配置）
set(CMAKE_C_FLAGS_INIT "-mthumb -O0 -g -gdwarf-4")
set(CMAKE_CXX_FLAGS_INIT "-mthumb -O0 -g -gdwarf-4")
set(CMAKE_ASM_FLAGS_INIT "-mthumb -g")

# 调试信息输出
message(STATUS "ARM Cortex-M GCC toolchain configured:")
message(STATUS "  System Name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  System Architecture: ${CMAKE_SYSTEM_ARCH}")
message(STATUS "  Path Hint: ${ARM_NONE_EABI_GCC_PATH}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  ASM Compiler: ${CMAKE_ASM_COMPILER}")
message(STATUS "  Linker: ${CMAKE_LINKER}")
message(STATUS "  Object Copy: ${CMAKE_OBJCOPY}")
message(STATUS "  Size Utility: ${CMAKE_SIZE_UTIL}")
message(STATUS "  Archiver: ${CMAKE_AR}")
