# Toolchain file for ARM Compiler 6 (armclang)

# 设置CMake策略CMP0123为NEW，避免自动添加CPU/架构标志
if(POLICY CMP0123)
    cmake_policy(SET CMP0123 NEW)
endif()

# 可选 PATH 提示：通过 VS Code settings 注入 ARM_CLANG_PATH（cmake.configureSettings 或 cmake.environment）
set(ARM_CLANG_PATH "${ARM_CLANG_PATH}")
if(NOT ARM_CLANG_PATH AND DEFINED ENV{ARM_CLANG_PATH})
    set(ARM_CLANG_PATH "$ENV{ARM_CLANG_PATH}")
endif()
if(ARM_CLANG_PATH)
    set(ENV{PATH} "${ARM_CLANG_PATH};$ENV{PATH}")
    list(APPEND CMAKE_PROGRAM_PATH "${ARM_CLANG_PATH}")
endif()

# 使用标准可执行名；如需自定义，可在 VS Code settings 覆盖这些变量
if(ARM_CLANG_PATH)
    if(WIN32)
        file(TO_CMAKE_PATH "${ARM_CLANG_PATH}" ARM_CLANG_PATH_NORM)
        set(CMAKE_C_COMPILER   "${ARM_CLANG_PATH_NORM}/armclang.exe")
        set(CMAKE_CXX_COMPILER "${ARM_CLANG_PATH_NORM}/armclang.exe")
        set(CMAKE_ASM_COMPILER "${ARM_CLANG_PATH_NORM}/armclang.exe")
        set(CMAKE_OBJCOPY      "${ARM_CLANG_PATH_NORM}/fromelf.exe")
        set(CMAKE_OBJDUMP      "${ARM_CLANG_PATH_NORM}/fromelf.exe")
        set(CMAKE_SIZE_UTIL    "${ARM_CLANG_PATH_NORM}/fromelf.exe")
        set(CMAKE_AR           "${ARM_CLANG_PATH_NORM}/armar.exe")
        set(CMAKE_RANLIB       "${ARM_CLANG_PATH_NORM}/armar.exe")
        set(CMAKE_LINKER       "${ARM_CLANG_PATH_NORM}/armlink.exe")
        else()
        set(CMAKE_C_COMPILER   "${ARM_CLANG_PATH}/armclang")
        set(CMAKE_CXX_COMPILER "${ARM_CLANG_PATH}/armclang")
        set(CMAKE_ASM_COMPILER "${ARM_CLANG_PATH}/armclang")
        set(CMAKE_OBJCOPY      "${ARM_CLANG_PATH}/fromelf")
        set(CMAKE_OBJDUMP      "${ARM_CLANG_PATH}/fromelf")
        set(CMAKE_SIZE_UTIL    "${ARM_CLANG_PATH}/fromelf")
        set(CMAKE_AR           "${ARM_CLANG_PATH}/armar")
        set(CMAKE_RANLIB       "${ARM_CLANG_PATH}/armar")
        set(CMAKE_LINKER       "${ARM_CLANG_PATH}/armlink")
    endif()
else()
    set(CMAKE_C_COMPILER   armclang)
    set(CMAKE_CXX_COMPILER armclang)
    set(CMAKE_ASM_COMPILER armclang)
    set(CMAKE_OBJCOPY      fromelf)
    set(CMAKE_OBJDUMP      fromelf)
    set(CMAKE_SIZE_UTIL    fromelf)
    set(CMAKE_AR           armar)
    set(CMAKE_RANLIB       armar)
    set(CMAKE_LINKER       armlink)
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
set(CMAKE_C_FLAGS_INIT "--target=arm-arm-none-eabi -mthumb -O0 -g")
set(CMAKE_CXX_FLAGS_INIT "--target=arm-arm-none-eabi -mthumb -O0 -g")
set(CMAKE_ASM_FLAGS_INIT "--target=arm-arm-none-eabi -mthumb -masm=auto -g")

# 跳过编译器测试（交叉编译环境）
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_ASM_COMPILER_WORKS TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

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