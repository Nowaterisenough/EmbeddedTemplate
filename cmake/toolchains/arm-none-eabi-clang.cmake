# ARM Cortex-M Clang Toolchain (ARM Compiler 6)
# 支持: Cortex-M0/M0+/M3/M4/M7/M23/M33/M55/M85
# 目标: 裸机嵌入式系统 (无操作系统)

# 设置目标系统信息（必须在其他设置之前）
set(CMAKE_SYSTEM_NAME Generic)

# 设置CMake策略CMP0123为NEW，避免自动添加CPU/架构标志
if(POLICY CMP0123)
    cmake_policy(SET CMP0123 NEW)
endif()

# 根据 BOARD 设置 CPU
if(BOARD MATCHES "stm32f4")
    set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
    set(CMAKE_EXE_LINKER_FLAGS_INIT "--cpu=cortex-m4")
elseif(BOARD MATCHES "stm32h7")  
    set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
    set(CMAKE_EXE_LINKER_FLAGS_INIT "--cpu=cortex-m7")
else()
    set(CMAKE_SYSTEM_PROCESSOR cortex-m7)
    set(CMAKE_EXE_LINKER_FLAGS_INIT "--cpu=cortex-m7")
endif()

# 跳过编译器测试（交叉编译环境）
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_ASM_COMPILER_WORKS TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 可选 PATH 提示：通过 VS Code settings 注入 ARM_NONE_EABI_CLANG_PATH
set(ARM_NONE_EABI_CLANG_PATH "${ARM_NONE_EABI_CLANG_PATH}")
if(NOT ARM_NONE_EABI_CLANG_PATH AND DEFINED ENV{ARM_NONE_EABI_CLANG_PATH})
    set(ARM_NONE_EABI_CLANG_PATH "$ENV{ARM_NONE_EABI_CLANG_PATH}")
endif()
# 向后兼容旧变量名
if(NOT ARM_NONE_EABI_CLANG_PATH AND ARM_CLANG_PATH)
    set(ARM_NONE_EABI_CLANG_PATH "${ARM_CLANG_PATH}")
endif()
if(NOT ARM_NONE_EABI_CLANG_PATH AND DEFINED ENV{ARM_CLANG_PATH})
    set(ARM_NONE_EABI_CLANG_PATH "$ENV{ARM_CLANG_PATH}")
endif()

if(ARM_NONE_EABI_CLANG_PATH)
    set(ENV{PATH} "${ARM_NONE_EABI_CLANG_PATH};$ENV{PATH}")
    list(APPEND CMAKE_PROGRAM_PATH "${ARM_NONE_EABI_CLANG_PATH}")
    message(STATUS "  Added ARM_NONE_EABI_CLANG_PATH to PATH: ${ARM_NONE_EABI_CLANG_PATH}")
endif()

# 设置编译器和工具
if(ARM_NONE_EABI_CLANG_PATH)
    if(WIN32)
        file(TO_CMAKE_PATH "${ARM_NONE_EABI_CLANG_PATH}" ARM_NONE_EABI_CLANG_PATH_NORM)
        set(CMAKE_C_COMPILER   "${ARM_NONE_EABI_CLANG_PATH_NORM}/armclang.exe")
        set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_CLANG_PATH_NORM}/armclang.exe")
        set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_CLANG_PATH_NORM}/armclang.exe")
        set(CMAKE_OBJCOPY      "${ARM_NONE_EABI_CLANG_PATH_NORM}/fromelf.exe")
        set(CMAKE_OBJDUMP      "${ARM_NONE_EABI_CLANG_PATH_NORM}/fromelf.exe")
        set(CMAKE_SIZE_UTIL    "${ARM_NONE_EABI_CLANG_PATH_NORM}/fromelf.exe")
        set(CMAKE_AR           "${ARM_NONE_EABI_CLANG_PATH_NORM}/armar.exe")
        set(CMAKE_RANLIB       "${ARM_NONE_EABI_CLANG_PATH_NORM}/armar.exe")
        set(CMAKE_LINKER       "${ARM_NONE_EABI_CLANG_PATH_NORM}/armlink.exe")
    else()
        set(CMAKE_C_COMPILER   "${ARM_NONE_EABI_CLANG_PATH}/armclang")
        set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_CLANG_PATH}/armclang")
        set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_CLANG_PATH}/armclang")
        set(CMAKE_OBJCOPY      "${ARM_NONE_EABI_CLANG_PATH}/fromelf")
        set(CMAKE_OBJDUMP      "${ARM_NONE_EABI_CLANG_PATH}/fromelf")
        set(CMAKE_SIZE_UTIL    "${ARM_NONE_EABI_CLANG_PATH}/fromelf")
        set(CMAKE_AR           "${ARM_NONE_EABI_CLANG_PATH}/armar")
        set(CMAKE_RANLIB       "${ARM_NONE_EABI_CLANG_PATH}/armar")
        set(CMAKE_LINKER       "${ARM_NONE_EABI_CLANG_PATH}/armlink")
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

# 设置默认编译标志
set(CMAKE_C_FLAGS_INIT "--target=arm-arm-none-eabi -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -O0 -g -gdwarf-4")
set(CMAKE_CXX_FLAGS_INIT "--target=arm-arm-none-eabi -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -O0 -g -gdwarf-4")
set(CMAKE_ASM_FLAGS_INIT "--target=arm-arm-none-eabi -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -masm=auto -g")

# 设置标准库为空（嵌入式系统）
set(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_CXX_STANDARD_LIBRARIES "")

# 自定义链接命令
set(CMAKE_C_LINK_EXECUTABLE 
    "<CMAKE_LINKER> <OBJECTS> <LINK_OPTIONS> <LINK_LIBRARIES> --output=<TARGET>")
set(CMAKE_CXX_LINK_EXECUTABLE 
    "<CMAKE_LINKER> <OBJECTS> <LINK_OPTIONS> <LINK_LIBRARIES> --output=<TARGET>")

# 调试信息输出
message(STATUS "ARM Cortex-M Clang toolchain configured:")
message(STATUS "  System Name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  System Architecture: ${CMAKE_SYSTEM_ARCH}")
message(STATUS "  Path Hint: ${ARM_NONE_EABI_CLANG_PATH}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  ASM Compiler: ${CMAKE_ASM_COMPILER}")
message(STATUS "  Linker: ${CMAKE_LINKER}")
message(STATUS "  Object Copy: ${CMAKE_OBJCOPY}")
message(STATUS "  Size Utility: ${CMAKE_SIZE_UTIL}")
message(STATUS "  Archiver: ${CMAKE_AR}")
