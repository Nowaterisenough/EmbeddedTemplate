/**
 * @file    hylink_protocol.h
 * @brief   HYlink协议定义 (V2.30)
 * @author  EmbeddedTemplate
 *
 * 协议说明:
 * - 包头: 11字节固定格式
 * - 包体: 可变长度,最大1024字节
 * - 校验: 包头累加和 + 包体CRC16
 */

#ifndef HYLINK_PROTOCOL_H
#define HYLINK_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * 协议常量
 * ======================================================================== */

#define HYLINK_SYNC_WORD_L      0xBB
#define HYLINK_SYNC_WORD_H      0xAA
#define HYLINK_HEADER_SIZE      11
#define HYLINK_MAX_DATA_SIZE    1024

/* ========================================================================
 * 设备ID定义 (2.4节)
 * ======================================================================== */

typedef enum {
    DEVICE_BROADCAST       = 0,   /* 广播 */
    DEVICE_GROUND_STATION  = 1,   /* 地面站 */
    DEVICE_COCKPIT         = 2,   /* 地面驾驶舱 */
    DEVICE_FLIGHT_CONTROL  = 5,   /* 飞控电路 */
    DEVICE_IO_CIRCUIT      = 6,   /* IO电路 */
    DEVICE_AIRCRAFT        = 10,  /* 大气机 */
    DEVICE_RECORDER        = 15,  /* 记录仪 */
    DEVICE_INS             = 45,  /* 惯导 */
    DEVICE_MEMS            = 50,  /* MEMS */
    DEVICE_DATALINK        = 55,  /* 数据链 */
    DEVICE_RADAR_ALT       = 60,  /* 雷达高度计 */
    DEVICE_BMS             = 65,  /* BMS */
    DEVICE_NAV_LIGHT       = 70,  /* 航灯 */
} hylink_device_id_t;

/* ========================================================================
 * 命令码定义 (3节)
 * ======================================================================== */

typedef enum {
    /* 系统基础指令 (0x00-0x0F) */
    CMD_HEARTBEAT           = 0x00,  /* 心跳包 */
    CMD_REQUEST             = 0x01,  /* 消息请求 */
    CMD_ACK                 = 0x02,  /* 消息确认 */
    CMD_HANDSHAKE           = 0x0E,  /* 通信握手 */
    CMD_SYSTEM_TIME         = 0x0F,  /* 系统时间戳 */

    /* 飞行数据指令 (0x10-0x1F) */
    CMD_POSITION_DATA       = 0x10,  /* 位置信息 */
    CMD_ATTITUDE_DATA       = 0x11,  /* 姿态信息 */
    CMD_VELOCITY_NED        = 0x13,  /* NED速度 */
    CMD_AIRSPEED_DATA       = 0x15,  /* 航空速度 */

    /* 控制指令 (0x20-0x2F) */
    CMD_JOYSTICK_CONTROL    = 0x20,  /* 摇杆控制 */

    /* 电池指令 (0x30-0x3F) */
    CMD_BATTERY_SYSTEM      = 0x30,  /* 电池系统 */

    /* 融合包体 (0xF0-0xFF) */
    CMD_FUSION_PACKET       = 0xFE,  /* 融合包体 */
} hylink_cmd_t;

/* ========================================================================
 * 数据包结构 (2.2节)
 * ======================================================================== */

/**
 * HYlink包头 (11字节)
 */
typedef struct __attribute__((packed)) {
    uint8_t  sync_word_l;      /* 0x00: 同步字低位 (0xBB) */
    uint8_t  sync_word_h;      /* 0x01: 同步字高位 (0xAA) */
    uint8_t  length_l;         /* 0x02: 包总长度低字节 */
    uint8_t  length_h;         /* 0x03: 包总长度高字节 */
    uint8_t  device_id;        /* 0x04: 源设备ID */
    uint8_t  seq_number;       /* 0x05: 帧序号 */
    uint8_t  cmd;              /* 0x06: 命令码 */
    uint8_t  reserved;         /* 0x07: 保留字段 */
    uint8_t  check_data_l;     /* 0x08: 数据CRC16低字节 */
    uint8_t  check_data_h;     /* 0x09: 数据CRC16高字节 */
    uint8_t  check_header;     /* 0x0A: 包头校验 */
} hylink_header_t;

/**
 * 完整数据包
 */
typedef struct {
    hylink_header_t header;
    uint8_t         data[HYLINK_MAX_DATA_SIZE];
    uint16_t        data_len;  /* 实际数据长度 */
} hylink_packet_t;

/* ========================================================================
 * 辅助宏
 * ======================================================================== */

/**
 * 获取包总长度 (包头+包体)
 */
#define HYLINK_GET_LENGTH(header) \
    ((uint16_t)((header)->length_h << 8 | (header)->length_l))

/**
 * 设置包总长度
 */
#define HYLINK_SET_LENGTH(header, len) do { \
    (header)->length_l = (uint8_t)((len) & 0xFF); \
    (header)->length_h = (uint8_t)(((len) >> 8) & 0xFF); \
} while(0)

/**
 * 获取数据CRC16
 */
#define HYLINK_GET_DATA_CRC(header) \
    ((uint16_t)((header)->check_data_h << 8 | (header)->check_data_l))

/**
 * 设置数据CRC16
 */
#define HYLINK_SET_DATA_CRC(header, crc) do { \
    (header)->check_data_l = (uint8_t)((crc) & 0xFF); \
    (header)->check_data_h = (uint8_t)(((crc) >> 8) & 0xFF); \
} while(0)

#ifdef __cplusplus
}
#endif

#endif /* HYLINK_PROTOCOL_H */
