# GetGitVersion.cmake
# 从 Git 仓库获取版本信息

# 获取 Git commit hash
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# 获取短 commit hash (7位)
execute_process(
    COMMAND git rev-parse --short=7 HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# 获取当前分支
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# 检查工作区是否有未提交的修改
execute_process(
    COMMAND git diff-index --quiet HEAD --
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_DIRTY_RESULT
    ERROR_QUIET
)

if(GIT_DIRTY_RESULT EQUAL 0)
    set(GIT_DIRTY 0)
else()
    set(GIT_DIRTY 1)
endif()

# 获取最新的 Git tag (语义化版本)
execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# 解析版本号 (支持 v1.2.3 或 1.2.3 格式)
if(GIT_TAG)
    string(REGEX MATCH "v?([0-9]+)\\.([0-9]+)\\.([0-9]+)" VERSION_MATCH "${GIT_TAG}")
    if(VERSION_MATCH)
        set(VERSION_MAJOR ${CMAKE_MATCH_1})
        set(VERSION_MINOR ${CMAKE_MATCH_2})
        set(VERSION_PATCH ${CMAKE_MATCH_3})
    else()
        set(VERSION_MAJOR 1)
        set(VERSION_MINOR 0)
        set(VERSION_PATCH 0)
    endif()
else()
    # 没有 tag，使用默认版本
    set(VERSION_MAJOR 1)
    set(VERSION_MINOR 0)
    set(VERSION_PATCH 0)
endif()

# 获取从最近 tag 以来的 commit 数量作为 build number
execute_process(
    COMMAND git rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE BUILD_NUMBER
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(NOT BUILD_NUMBER)
    set(BUILD_NUMBER 0)
endif()

# 获取 Unix 时间戳
string(TIMESTAMP BUILD_TIMESTAMP "%s" UTC)

# 获取格式化的日期和时间
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME "%H:%M:%S")

# 如果 Git 命令失败，设置默认值
if(NOT GIT_COMMIT_HASH)
    set(GIT_COMMIT_HASH "unknown")
endif()

if(NOT GIT_COMMIT_SHORT)
    set(GIT_COMMIT_SHORT "unknown")
endif()

if(NOT GIT_BRANCH)
    set(GIT_BRANCH "unknown")
endif()

# 打印版本信息
message(STATUS "================================================================================")
message(STATUS "  Firmware Version Information")
message(STATUS "================================================================================")
message(STATUS "  Version:       ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message(STATUS "  Build Number:  ${BUILD_NUMBER}")
message(STATUS "  Git Branch:    ${GIT_BRANCH}")
message(STATUS "  Git Commit:    ${GIT_COMMIT_SHORT}")
message(STATUS "  Git Dirty:     ${GIT_DIRTY}")
message(STATUS "  Build Date:    ${BUILD_DATE} ${BUILD_TIME}")
message(STATUS "================================================================================")
