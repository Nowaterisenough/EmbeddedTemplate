# 若外部未指定 BOARD，则给一个默认
if(NOT DEFINED BOARD)
  set(BOARD "stm32h743zi" CACHE STRING "Target board")
endif()

set(THIS_BOARD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}")

if(NOT EXISTS "${THIS_BOARD_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "Board '${BOARD}' not found at ${THIS_BOARD_DIR}")
endif()

# 让顶层和其他子目录都能看到 BOARD_DIR
set(BOARD_DIR "${THIS_BOARD_DIR}" PARENT_SCOPE)

# 进入具体板子目录编译 hal_${BOARD} / board_${BOARD}
add_subdirectory(${BOARD})