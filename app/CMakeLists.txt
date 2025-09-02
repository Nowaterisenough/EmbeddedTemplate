# app/CMakeLists.txt

add_executable(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# 产物根目录：放到构建目录(build)下的 bin/BOARD/
set(OUTPUT_ROOT "${CMAKE_BINARY_DIR}")
set(OUTPUT_DIR  "${OUTPUT_ROOT}/bin/${BOARD}")

# 确保 .elf/.lib 等都输出到 OUTPUT_DIR
set_target_properties(${PROJECT_NAME} PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}"
  SUFFIX ".elf"
  RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
)

# app 头目录（板的 config/宏由 board:: 传播，无需手动加）
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接板级与模块（板级会间接链接 stm32h7_hal 与 cubeh7）  
target_link_libraries(${PROJECT_NAME} PRIVATE
  board::${BOARD}
  RTT
)

# 通用的输入/输出文件（用生成表达式获取 .elf 的最终路径）
set(INPUT_ELF "$<TARGET_FILE:${PROJECT_NAME}>")
set(OUT_BIN   "${OUTPUT_DIR}/${PROJECT_NAME}.bin")
set(OUT_HEX   "${OUTPUT_DIR}/${PROJECT_NAME}.hex")

# 产物（bin/hex）与 size（区分 ARMClang / GCC）
if(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
  # 查找 fromelf（避免与工具链变量名冲突）
  set(FROMELF_EXE "")
  # 优先根据编译器目录作为 hint
  set(_CLANG_HINT_DIR "")
  if(CMAKE_C_COMPILER)
    get_filename_component(_CLANG_HINT_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
  endif()
  if(_CLANG_HINT_DIR)
    find_program(FROMELF_EXE NAMES fromelf HINTS "${_CLANG_HINT_DIR}" NO_DEFAULT_PATH)
  endif()
  if(NOT FROMELF_EXE)
    find_program(FROMELF_EXE NAMES fromelf REQUIRED)
  endif()

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
    COMMAND "${FROMELF_EXE}" --bin "${INPUT_ELF}" -o "${OUT_BIN}"
    COMMAND "${FROMELF_EXE}" --i32 "${INPUT_ELF}" -o "${OUT_HEX}"
    BYPRODUCTS "${OUT_BIN}" "${OUT_HEX}"
    COMMENT "Generating .bin/.hex into ${OUTPUT_DIR}"
  )

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${FROMELF_EXE}" --text -c "${INPUT_ELF}"
    COMMENT "Size (fromelf)"
  )
else()
  # 尝试根据 C 编译器的目录作为提示查找工具
  set(_TOOL_HINT_DIR "")
  if(CMAKE_C_COMPILER)
    get_filename_component(_TOOL_HINT_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
  endif()

  # 单独变量，避免与工具链中 CMAKE_OBJCOPY 普通变量名冲突
  if(_TOOL_HINT_DIR)
    find_program(OBJCOPY_EXE NAMES arm-none-eabi-objcopy HINTS "${_TOOL_HINT_DIR}" NO_DEFAULT_PATH)
    find_program(CMAKE_SIZE  NAMES arm-none-eabi-size    HINTS "${_TOOL_HINT_DIR}" NO_DEFAULT_PATH)
  endif()

  if(NOT OBJCOPY_EXE)
    find_program(OBJCOPY_EXE NAMES arm-none-eabi-objcopy REQUIRED)
  endif()
  if(NOT CMAKE_SIZE)
    find_program(CMAKE_SIZE NAMES arm-none-eabi-size REQUIRED)
  endif()

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
    COMMAND "${OBJCOPY_EXE}" -O binary "${INPUT_ELF}" "${OUT_BIN}"
    COMMAND "${OBJCOPY_EXE}" -O ihex   "${INPUT_ELF}" "${OUT_HEX}"
    BYPRODUCTS "${OUT_BIN}" "${OUT_HEX}"
    COMMENT "Generating .bin/.hex into ${OUTPUT_DIR}"
  )

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} --format=berkeley "${INPUT_ELF}"
    COMMENT "Size"
  )
endif()