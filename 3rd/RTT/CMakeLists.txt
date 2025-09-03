# 3rd/RTT/CMakeLists.txt
# RTT (Real-Time Transfer) 库，带 printf 重定向

# 仅用 C 源，避免汇编语法差异
set(RTT_SOURCES
  RTT/SEGGER_RTT.c
  RTT/SEGGER_RTT_printf.c
)

# 头文件目录
set(RTT_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/RTT
  ${CMAKE_CURRENT_SOURCE_DIR}/Config
)

# 根据编译器选择合适的 Syscalls 源，实现 printf 重定向到 RTT
set(RTT_SYSCALLS "")
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Syscalls/SEGGER_RTT_Syscalls_KEIL.c")
    set(RTT_SYSCALLS Syscalls/SEGGER_RTT_Syscalls_KEIL.c)
  endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Syscalls/SEGGER_RTT_Syscalls_GCC.c")
    set(RTT_SYSCALLS Syscalls/SEGGER_RTT_Syscalls_GCC.c)
  endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "IAR")
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Syscalls/SEGGER_RTT_Syscalls_IAR.c")
    set(RTT_SYSCALLS Syscalls/SEGGER_RTT_Syscalls_IAR.c)
  endif()
endif()

if(RTT_SYSCALLS STREQUAL "")
  message(WARNING
    "未找到匹配当前编译器(${CMAKE_C_COMPILER_ID})的 RTT Syscalls 源文件，"
    "将不会重定向 printf 到 RTT。你可以手动指定 RTT_SYSCALLS。"
  )
else()
  message(STATUS "RTT printf 重定向: 使用 ${RTT_SYSCALLS}")
  list(APPEND RTT_SOURCES ${RTT_SYSCALLS})
endif()

# 创建 RTT 静态库目标
add_library(RTT STATIC ${RTT_SOURCES})

# 头文件
target_include_directories(RTT PUBLIC ${RTT_INCLUDE_DIRS})

# 编译定义：默认非阻塞丢弃
target_compile_definitions(RTT PUBLIC
  SEGGER_RTT_MODE_DEFAULT=SEGGER_RTT_MODE_NO_BLOCK_SKIP
)

# 第三方库适度关闭告警，并仅对 C 源做尺寸优化
target_compile_options(RTT PRIVATE
  $<$<COMPILE_LANGUAGE:C>:-Os>
  $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
  $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
  # 可按需加额外抑制，例如 -w
)

# 应用板级CPU编译选项（如果定义了）
if(DEFINED BOARD_COMPILE_OPTIONS)
  target_compile_options(RTT PRIVATE ${BOARD_COMPILE_OPTIONS})
endif()

set_target_properties(RTT PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED ON
)