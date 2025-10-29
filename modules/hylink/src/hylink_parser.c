/**
 * @file    hylink_parser.c
 * @brief   HYlink协议解析器实现
 */

#include "hylink_parser.h"
#include <string.h>

/* ========================================================================
 * CRC16-CCITT查表法 (多项式0x1021)
 * ======================================================================== */

static const uint16_t CRC16_TABLE[256] = {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50A5,0x60C6,0x70E7,
    0x8108,0x9129,0xA14A,0xB16B,0xC18C,0xD1AD,0xE1CE,0xF1EF,
    0x1231,0x0210,0x3273,0x2252,0x52B5,0x4294,0x72F7,0x62D6,
    0x9339,0x8318,0xB37B,0xA35A,0xD3BD,0xC39C,0xF3FF,0xE3DE,
    0x2462,0x3443,0x0420,0x1401,0x64E6,0x74C7,0x44A4,0x5485,
    0xA56A,0xB54B,0x8528,0x9509,0xE5EE,0xF5CF,0xC5AC,0xD58D,
    0x3653,0x2672,0x1611,0x0630,0x76D7,0x66F6,0x5695,0x46B4,
    0xB75B,0xA77A,0x9719,0x8738,0xF7DF,0xE7FE,0xD79D,0xC7BC,
    0x48C4,0x58E5,0x6886,0x78A7,0x0840,0x1861,0x2802,0x3823,
    0xC9CC,0xD9ED,0xE98E,0xF9AF,0x8948,0x9969,0xA90A,0xB92B,
    0x5AF5,0x4AD4,0x7AB7,0x6A96,0x1A71,0x0A50,0x3A33,0x2A12,
    0xDBFD,0xCBDC,0xFBBF,0xEB9E,0x9B79,0x8B58,0xBB3B,0xAB1A,
    0x6CA6,0x7C87,0x4CE4,0x5CC5,0x2C22,0x3C03,0x0C60,0x1C41,
    0xEDAE,0xFD8F,0xCDEC,0xDDCD,0xAD2A,0xBD0B,0x8D68,0x9D49,
    0x7E97,0x6EB6,0x5ED5,0x4EF4,0x3E13,0x2E32,0x1E51,0x0E70,
    0xFF9F,0xEFBE,0xDFDD,0xCFFC,0xBF1B,0xAF3A,0x9F59,0x8F78,
    0x9188,0x81A9,0xB1CA,0xA1EB,0xD10C,0xC12D,0xF14E,0xE16F,
    0x1080,0x00A1,0x30C2,0x20E3,0x5004,0x4025,0x7046,0x6067,
    0x83B9,0x9398,0xA3FB,0xB3DA,0xC33D,0xD31C,0xE37F,0xF35E,
    0x02B1,0x1290,0x22F3,0x32D2,0x4235,0x5214,0x6277,0x7256,
    0xB5EA,0xA5CB,0x95A8,0x8589,0xF56E,0xE54F,0xD52C,0xC50D,
    0x34E2,0x24C3,0x14A0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xA7DB,0xB7FA,0x8799,0x97B8,0xE75F,0xF77E,0xC71D,0xD73C,
    0x26D3,0x36F2,0x0691,0x16B0,0x6657,0x7676,0x4615,0x5634,
    0xD94C,0xC96D,0xF90E,0xE92F,0x99C8,0x89E9,0xB98A,0xA9AB,
    0x5844,0x4865,0x7806,0x6827,0x18C0,0x08E1,0x3882,0x28A3,
    0xCB7D,0xDB5C,0xEB3F,0xFB1E,0x8BF9,0x9BD8,0xABBB,0xBB9A,
    0x4A75,0x5A54,0x6A37,0x7A16,0x0AF1,0x1AD0,0x2AB3,0x3A92,
    0xFD2E,0xED0F,0xDD6C,0xCD4D,0xBDAA,0xAD8B,0x9DE8,0x8DC9,
    0x7C26,0x6C07,0x5C64,0x4C45,0x3CA2,0x2C83,0x1CE0,0x0CC1,
    0xEF1F,0xFF3E,0xCF5D,0xDF7C,0xAF9B,0xBFBA,0x8FD9,0x9FF8,
    0x6E17,0x7E36,0x4E55,0x5E74,0x2E93,0x3EB2,0x0ED1,0x1EF0
};

uint16_t hylink_calc_crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < len; i++) {
        uint8_t idx = (uint8_t)((crc >> 8) ^ data[i]);
        crc = (uint16_t)((crc << 8) ^ CRC16_TABLE[idx]);
    }

    return crc;
}

uint8_t hylink_calc_header_checksum(const hylink_header_t *header)
{
    const uint8_t *bytes = (const uint8_t *)header;
    uint8_t checksum = 0;

    /* 累加前10个字节 (不包括check_header本身) */
    for (int i = 0; i < 10; i++) {
        checksum += bytes[i];
    }

    return checksum;
}

/* ========================================================================
 * 解析器状态机
 * ======================================================================== */

typedef enum {
    STATE_IDLE,           /* 空闲,等待同步字 */
    STATE_SYNC_L,         /* 收到SYNC_L,等待SYNC_H */
    STATE_HEADER,         /* 接收包头 */
    STATE_DATA,           /* 接收数据 */
} parser_state_t;

typedef struct {
    parser_state_t         state;
    hylink_packet_t        packet;
    uint16_t               rx_count;      /* 当前接收字节计数 */
    uint16_t               expected_len;  /* 期望的数据长度 */
    hylink_packet_callback_t callback;
    hylink_parser_stats_t  stats;
} parser_context_t;

static parser_context_t g_parser;

/* ========================================================================
 * 内部函数
 * ======================================================================== */

/**
 * 验证包头
 */
static bool validate_header(const hylink_header_t *header)
{
    /* 1. 检查同步字 */
    if (header->sync_word_l != HYLINK_SYNC_WORD_L ||
        header->sync_word_h != HYLINK_SYNC_WORD_H) {
        return false;
    }

    /* 2. 检查长度合法性 */
    uint16_t total_len = HYLINK_GET_LENGTH(header);
    if (total_len < HYLINK_HEADER_SIZE ||
        total_len > (HYLINK_HEADER_SIZE + HYLINK_MAX_DATA_SIZE)) {
        return false;
    }

    /* 3. 检查包头校验和 */
    uint8_t calc_checksum = hylink_calc_header_checksum(header);
    if (calc_checksum != header->check_header) {
        return false;
    }

    return true;
}

/**
 * 验证数据包
 */
static bool validate_packet(const hylink_packet_t *packet)
{
    /* 检查数据CRC */
    uint16_t calc_crc = hylink_calc_crc16(packet->data, packet->data_len);
    uint16_t recv_crc = HYLINK_GET_DATA_CRC(&packet->header);

    return (calc_crc == recv_crc);
}

/**
 * 处理完整数据包
 */
static void handle_complete_packet(parser_context_t *ctx)
{
    /* 验证数据CRC */
    if (!validate_packet(&ctx->packet)) {
        ctx->stats.crc_errors++;
        return;
    }

    /* 统计 */
    ctx->stats.total_packets++;

    /* 回调通知 */
    if (ctx->callback) {
        ctx->callback(&ctx->packet);
    }
}

/**
 * 状态机复位
 */
static void parser_reset_internal(parser_context_t *ctx)
{
    ctx->state       = STATE_IDLE;
    ctx->rx_count    = 0;
    ctx->expected_len = 0;
}

/**
 * 处理单字节
 */
static void process_byte(parser_context_t *ctx, uint8_t byte)
{
    uint8_t *raw_header = (uint8_t *)&ctx->packet.header;

    switch (ctx->state) {
        case STATE_IDLE:
            if (byte == HYLINK_SYNC_WORD_L) {
                ctx->state = STATE_SYNC_L;
                raw_header[0] = byte;
                ctx->rx_count = 1;
            }
            break;

        case STATE_SYNC_L:
            if (byte == HYLINK_SYNC_WORD_H) {
                raw_header[1] = byte;
                ctx->rx_count = 2;
                ctx->state = STATE_HEADER;
            } else {
                /* 同步失败,重新寻找SYNC_L */
                if (byte == HYLINK_SYNC_WORD_L) {
                    ctx->state = STATE_SYNC_L;
                    raw_header[0] = byte;
                    ctx->rx_count = 1;
                } else {
                    parser_reset_internal(ctx);
                }
            }
            break;

        case STATE_HEADER:
            raw_header[ctx->rx_count++] = byte;

            if (ctx->rx_count == HYLINK_HEADER_SIZE) {
                /* 包头接收完成,验证 */
                if (!validate_header(&ctx->packet.header)) {
                    ctx->stats.header_errors++;
                    parser_reset_internal(ctx);
                    break;
                }

                /* 计算数据长度 */
                uint16_t total_len = HYLINK_GET_LENGTH(&ctx->packet.header);
                ctx->expected_len = total_len - HYLINK_HEADER_SIZE;

                if (ctx->expected_len == 0) {
                    /* 无数据包体,直接处理 */
                    ctx->packet.data_len = 0;
                    handle_complete_packet(ctx);
                    parser_reset_internal(ctx);
                } else {
                    /* 继续接收数据 */
                    ctx->state = STATE_DATA;
                    ctx->rx_count = 0;
                }
            }
            break;

        case STATE_DATA:
            ctx->packet.data[ctx->rx_count++] = byte;

            if (ctx->rx_count == ctx->expected_len) {
                /* 数据接收完成 */
                ctx->packet.data_len = ctx->expected_len;
                handle_complete_packet(ctx);
                parser_reset_internal(ctx);
            }
            break;

        default:
            parser_reset_internal(ctx);
            break;
    }
}

/* ========================================================================
 * 公共API实现
 * ======================================================================== */

void hylink_parser_init(hylink_packet_callback_t callback)
{
    memset(&g_parser, 0, sizeof(g_parser));
    g_parser.callback = callback;
    g_parser.state = STATE_IDLE;
}

void hylink_parser_feed(const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        process_byte(&g_parser, data[i]);
    }
}

void hylink_parser_reset(void)
{
    parser_reset_internal(&g_parser);
}

void hylink_parser_get_stats(hylink_parser_stats_t *stats)
{
    if (stats) {
        *stats = g_parser.stats;
    }
}
