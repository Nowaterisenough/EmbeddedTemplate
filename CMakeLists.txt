cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0123 NEW)

project(EmbeddedTemplate C ASM)  # 启用 C 与 ASM

# 添加子目录，使用模块化构建
add_subdirectory(drivers)
add_subdirectory(3rd)
add_subdirectory(boards)
add_subdirectory(modules)
add_subdirectory(app)

