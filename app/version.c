/**
 * @file    version.c
 * @brief   固件版本信息实现
 * @note    构建时通过 CMake 自动注入版本信息
 */

#include "version.h"
#include <stdio.h>

/* 构建时注入的宏定义（通过 CMake 传递） */
#ifndef FW_VERSION_MAJOR
#define FW_VERSION_MAJOR 0
#endif

#ifndef FW_VERSION_MINOR
#define FW_VERSION_MINOR 1
#endif

#ifndef FW_VERSION_PATCH
#define FW_VERSION_PATCH 0
#endif

#ifndef FW_BUILD_NUMBER
#define FW_BUILD_NUMBER 0
#endif

#ifndef FW_GIT_COMMIT
#define FW_GIT_COMMIT "unknown"
#endif

#ifndef FW_GIT_BRANCH
#define FW_GIT_BRANCH "unknown"
#endif

#ifndef FW_GIT_DIRTY
#define FW_GIT_DIRTY 0
#endif

#ifndef FW_BUILD_DATE
#define FW_BUILD_DATE __DATE__
#endif

#ifndef FW_BUILD_TIME
#define FW_BUILD_TIME __TIME__
#endif

#ifndef FW_BUILD_TIMESTAMP
#define FW_BUILD_TIMESTAMP 0
#endif

#ifndef FW_BOARD_NAME
#define FW_BOARD_NAME "unknown"
#endif

/* 编译器信息 */
#if defined(__GNUC__)
    #define COMPILER_STRING "GCC " __VERSION__
#elif defined(__clang__)
    #define COMPILER_STRING "Clang " __clang_version__
#elif defined(__ARMCC_VERSION)
    #define COMPILER_STRING "ARMCC " __ARMCC_VERSION
#else
    #define COMPILER_STRING "Unknown"
#endif

/* ========================================================================
 * 固件版本信息 (存储在 .version section)
 * ======================================================================== */

/**
 * 固件版本信息常量
 * 存储在 Flash 的固定位置，可通过以下方式读取：
 * 1. GDB: x/64x &firmware_version
 * 2. objdump: objdump -s -j .version firmware.elf
 * 3. 自定义工具扫描二进制文件
 */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

const firmware_version_t firmware_version __attribute__((section(".version"), used)) = {
    .magic            = FW_VERSION_MAGIC,
    .major            = FW_VERSION_MAJOR,
    .minor            = FW_VERSION_MINOR,
    .patch            = FW_VERSION_PATCH,
    .build_number     = FW_BUILD_NUMBER,
    .git_commit       = TOSTRING(FW_GIT_COMMIT),
    .git_branch       = TOSTRING(FW_GIT_BRANCH),
    .is_dirty         = FW_GIT_DIRTY,
    .build_date       = TOSTRING(FW_BUILD_DATE),
    .build_time       = TOSTRING(FW_BUILD_TIME),
    .build_timestamp  = FW_BUILD_TIMESTAMP,
    .compiler         = COMPILER_STRING,
    .board_name       = TOSTRING(FW_BOARD_NAME),
    .crc32            = 0,  /* TODO: 实现 CRC32 计算 */
};

/* ========================================================================
 * 版本信息接口实现
 * ======================================================================== */

/**
 * 静态缓冲区用于格式化版本字符串
 */
static char version_string_buffer[128];

const char* version_get_string(void)
{
    /* 格式: v1.2.3+build.123.abc1234-dirty */
    snprintf(version_string_buffer, sizeof(version_string_buffer),
             "v%u.%u.%u+build.%lu.%.7s%s",
             firmware_version.major,
             firmware_version.minor,
             firmware_version.patch,
             (unsigned long)firmware_version.build_number,
             firmware_version.git_commit,
             firmware_version.is_dirty ? "-dirty" : "");

    return version_string_buffer;
}

int version_is_valid(void)
{
    /* 验证魔法数字 */
    if (firmware_version.magic != FW_VERSION_MAGIC) {
        return 0;
    }

    /* TODO: 验证 CRC32 */

    return 1;
}

void version_print(void)
{
    if (!version_is_valid()) {
        printf("ERROR: Invalid firmware version information!\n");
        return;
    }

    printf("\n");
    printf("================================================================================\n");
    printf("  Firmware Version Information\n");
    printf("================================================================================\n");
    printf("  Version:       %s\n", version_get_string());
    printf("  Board:         %s\n", firmware_version.board_name);
    printf("  Build Date:    %s %s\n", firmware_version.build_date, firmware_version.build_time);
    printf("  Git Branch:    %s\n", firmware_version.git_branch);
    printf("  Git Commit:    %s\n", firmware_version.git_commit);
    printf("  Compiler:      %s\n", firmware_version.compiler);
    printf("================================================================================\n");
    printf("\n");
}
