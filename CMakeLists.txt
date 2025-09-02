cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0123 NEW)

# 必须在project()之前设置系统和处理器
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m7)

# 编译器设置由工具链文件处理

project(EmbeddedTemplate C ASM)  # 启用 C 与 ASM
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# 添加子目录，使用模块化构建
add_subdirectory(3rd)
add_subdirectory(drivers) 
add_subdirectory(boards)
add_subdirectory(app)

