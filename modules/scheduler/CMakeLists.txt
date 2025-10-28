# modules/scheduler/CMakeLists.txt
# 轻量级抢占式任务调度器

cmake_minimum_required(VERSION 3.20)

# 确定移植层架构
if(BOARD STREQUAL "stm32h743zi" OR BOARD STREQUAL "stm32f407zg")
    set(SCHEDULER_PORT "ARM_CM4F")
else()
    message(FATAL_ERROR "不支持的板子: ${BOARD}，请为其添加移植层")
endif()

# 创建调度器库
add_library(scheduler STATIC
    src/scheduler.c
    port/${SCHEDULER_PORT}/port.c
)

# 头文件路径
target_include_directories(scheduler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 依赖板级抽象层
target_link_libraries(scheduler PUBLIC
    board::${BOARD}
)

# 编译选项 (优化大小)
target_compile_options(scheduler PRIVATE
    -Os                    # 优化代码大小
    -ffunction-sections    # 函数独立段
    -fdata-sections        # 数据独立段
)

# 创建别名
add_library(scheduler::scheduler ALIAS scheduler)

# 可选：构建示例
option(BUILD_SCHEDULER_EXAMPLE "构建调度器示例" OFF)

if(BUILD_SCHEDULER_EXAMPLE)
    add_executable(scheduler_demo
        example/scheduler_demo.c
    )

    target_link_libraries(scheduler_demo PRIVATE
        scheduler::scheduler
        board::${BOARD}
        RTT
    )

    # 输出到独立目录
    set_target_properties(scheduler_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BOARD}/examples"
    )
endif()
