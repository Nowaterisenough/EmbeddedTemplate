# HYlink 协议解析模块

add_library(hylink STATIC
    src/hylink_parser.c
)

target_include_directories(hylink PUBLIC
    include
)

# 链接板级库以获取芯片定义和编译选项
target_link_libraries(hylink PUBLIC
    board::${BOARD}
)
