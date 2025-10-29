cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0123 NEW)

project(EmbeddedTemplate C ASM)  # 启用 C 与 ASM

# 确保 CMAKE_OBJCOPY 有完整路径（避免 post-build 找不到命令）
if(NOT CMAKE_OBJCOPY OR NOT EXISTS "${CMAKE_OBJCOPY}")
    get_filename_component(COMPILER_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    find_program(CMAKE_OBJCOPY
        NAMES arm-none-eabi-objcopy arm-none-eabi-objcopy.exe
        HINTS "${COMPILER_DIR}"
        NO_DEFAULT_PATH
        REQUIRED
    )
    if(NOT CMAKE_OBJCOPY)
        message(FATAL_ERROR "Cannot find arm-none-eabi-objcopy. Please check your toolchain installation.")
    endif()
    message(STATUS "Found CMAKE_OBJCOPY: ${CMAKE_OBJCOPY}")
endif()

# 添加子目录，使用模块化构建
add_subdirectory(drivers)
add_subdirectory(3rd)
add_subdirectory(boards)
add_subdirectory(modules)
add_subdirectory(app)

