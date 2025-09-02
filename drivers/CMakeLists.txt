# 汇总模块 - 根据 BOARD 值动态选择驱动
if(BOARD MATCHES "^stm32f4")
    add_subdirectory(STM32F4)
elseif(BOARD MATCHES "^stm32h7")  
    add_subdirectory(STM32H7)
else()
    message(FATAL_ERROR "不支持的板子: ${BOARD}。请添加对应的驱动子目录。")
endif()