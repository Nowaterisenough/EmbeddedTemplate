/**
 * @file    hylink_parser.h
 * @brief   HYlink协议解析器 - 状态机实现
 * @author  EmbeddedTemplate
 *
 * 设计原则:
 * - 状态机驱动: 逐字节解析,不依赖完整包
 * - 零拷贝: 解析完成后通过回调通知,避免额外拷贝
 * - 健壮性: 错误自动恢复,不会因单个错包导致后续包丢失
 */

#ifndef HYLINK_PARSER_H
#define HYLINK_PARSER_H

#include "hylink_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * 回调函数类型
 * ======================================================================== */

/**
 * 数据包接收回调
 *
 * @param packet  解析完成的数据包
 *
 * @note packet指针仅在回调期间有效,需要立即处理或拷贝
 */
typedef void (*hylink_packet_callback_t)(const hylink_packet_t *packet);

/* ========================================================================
 * 解析器API
 * ======================================================================== */

/**
 * 初始化解析器
 *
 * @param callback  数据包回调函数
 */
void hylink_parser_init(hylink_packet_callback_t callback);

/**
 * 喂数据给解析器
 *
 * @param data  数据指针
 * @param len   数据长度
 *
 * @note 可以逐字节喂入,也可以批量喂入
 * @note 线程安全: 不可在中断和主循环同时调用
 */
void hylink_parser_feed(const uint8_t *data, uint16_t len);

/**
 * 重置解析器
 */
void hylink_parser_reset(void);

/**
 * 获取解析统计信息
 */
typedef struct {
    uint32_t total_packets;     /* 成功解析的包总数 */
    uint32_t crc_errors;        /* CRC错误计数 */
    uint32_t header_errors;     /* 包头错误计数 */
    uint32_t length_errors;     /* 长度错误计数 */
} hylink_parser_stats_t;

void hylink_parser_get_stats(hylink_parser_stats_t *stats);

/* ========================================================================
 * 校验算法
 * ======================================================================== */

/**
 * 计算包头校验和 (累加和)
 *
 * @param header  包头指针
 * @return        校验和
 */
uint8_t hylink_calc_header_checksum(const hylink_header_t *header);

/**
 * 计算数据CRC16 (CRC16-CCITT)
 *
 * @param data  数据指针
 * @param len   数据长度
 * @return      CRC16值
 */
uint16_t hylink_calc_crc16(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* HYLINK_PARSER_H */
