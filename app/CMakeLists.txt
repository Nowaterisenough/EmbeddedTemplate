# app/CMakeLists.txt

add_executable(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.c  # 完整版(带UART)
  ${CMAKE_CURRENT_SOURCE_DIR}/syscalls.c
  # UART 实现已移到 boards/stm32h743zi/uart_port.c
)

# 产物根目录：放到构建目录(build)下的 bin/BOARD/
set(OUTPUT_ROOT "${CMAKE_BINARY_DIR}")
set(OUTPUT_DIR  "${OUTPUT_ROOT}/bin/${BOARD}")

# 确保 .elf/.lib 等都输出到 OUTPUT_DIR
set_target_properties(${PROJECT_NAME} PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}"
  # SUFFIX ".elf"
  RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
)

# app 头目录（板的 config/宏由 board:: 传播，无需手动加）
# UART 头文件也通过 board:: 传播
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接板级与模块（板级会间接链接 stm32h7_hal 与 cubeh7）
target_link_libraries(${PROJECT_NAME} PRIVATE
  board::${BOARD}
  RTT
  scheduler
  hylink
)

# 通用的输入/输出文件（用生成表达式获取 .elf 的最终路径）
set(INPUT_ELF "$<TARGET_FILE:${PROJECT_NAME}>")
set(OUT_BIN   "${OUTPUT_DIR}/${PROJECT_NAME}.bin")
set(OUT_HEX   "${OUTPUT_DIR}/${PROJECT_NAME}.hex")

# 产物（bin/hex）
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
    COMMAND "${CMAKE_OBJCOPY}" --i32  "$<TARGET_FILE:${PROJECT_NAME}>" --output "${OUT_HEX}"
    COMMAND "${CMAKE_OBJCOPY}" --bin  "$<TARGET_FILE:${PROJECT_NAME}>" --output "${OUT_BIN}"
    BYPRODUCTS "${OUT_BIN}" "${OUT_HEX}"
    COMMENT "fromelf: generate HEX/BIN into ${OUTPUT_DIR}"
  )
else()
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
    COMMAND "${CMAKE_OBJCOPY}" -O ihex   "$<TARGET_FILE:${PROJECT_NAME}>" "${OUT_HEX}"
    COMMAND "${CMAKE_OBJCOPY}" -O binary "$<TARGET_FILE:${PROJECT_NAME}>" "${OUT_BIN}"
    BYPRODUCTS "${OUT_BIN}" "${OUT_HEX}"
    COMMENT "objcopy: generate HEX/BIN into ${OUTPUT_DIR}"
  )
endif()