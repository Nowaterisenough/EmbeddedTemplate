# UART 驱动模块

add_library(uart_driver STATIC
    src/uart_driver.c
)

target_include_directories(uart_driver PUBLIC
    include
)

# 链接板级库以获取HAL和芯片定义
target_link_libraries(uart_driver PUBLIC
    board::${BOARD}
)
