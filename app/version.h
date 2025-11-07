/**
 * @file    version.h
 * @brief   固件版本信息定义
 * @note    版本号自动从 Git 标签生成，构建信息在编译时注入
 */

#ifndef VERSION_H
#define VERSION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * 版本信息结构
 * ======================================================================== */

/**
 * 固件版本信息结构
 * 存储在 Flash 的 .version section，可通过调试器或工具直接读取
 */
typedef struct {
    /* 魔法数字：用于验证版本信息的有效性 */
    uint32_t magic;                 /* 0x46574556 ("FWVE") */

    /* 版本号：语义化版本 (Semantic Versioning) */
    uint8_t  major;                 /* 主版本号 (不兼容的API修改) */
    uint8_t  minor;                 /* 次版本号 (向后兼容的功能性新增) */
    uint16_t patch;                 /* 修订号 (向后兼容的问题修正) */

    /* 构建信息 */
    uint32_t build_number;          /* 构建编号 (CI/CD 自动递增) */
    char     git_commit[41];        /* Git commit SHA-1 (40字符 + \0) */
    char     git_branch[32];        /* Git 分支名 */
    uint8_t  is_dirty;              /* Git 工作区是否有未提交修改 */
    uint8_t  reserved1[3];          /* 对齐 */

    /* 时间戳 */
    char     build_date[12];        /* 构建日期 YYYY-MM-DD\0 */
    char     build_time[9];         /* 构建时间 HH:MM:SS\0 */
    uint32_t build_timestamp;       /* Unix 时间戳 */

    /* 工具链信息 */
    char     compiler[32];          /* 编译器名称和版本 */
    char     board_name[32];        /* 目标板名称 */

    /* 校验 */
    uint32_t crc32;                 /* 整个结构的 CRC32 (不包括此字段) */
} firmware_version_t;

/* 版本魔法数字 */
#define FW_VERSION_MAGIC    0x46574556U  /* "FWVE" */

/* ========================================================================
 * 外部声明
 * ======================================================================== */

/**
 * 固件版本信息 (存储在 .version section)
 * 通过 version.c 定义，链接器放置到专用区域
 */
extern const firmware_version_t firmware_version;

/**
 * 获取固件版本字符串
 * @return 版本字符串，格式: "v1.2.3+build.123.abc1234"
 */
const char* version_get_string(void);

/**
 * 验证版本信息的有效性
 * @return 1=有效, 0=无效
 */
int version_is_valid(void);

/**
 * 打印版本信息到调试串口
 */
void version_print(void);

#ifdef __cplusplus
}
#endif

#endif /* VERSION_H */
