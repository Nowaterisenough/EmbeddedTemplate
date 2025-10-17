#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stddef.h>

/* 统一板级接口：跨板保持一致，不包含芯片族头/板宏 */
void     board_init(void);
void     board_delay_ms(uint32_t ms);
uint32_t board_millis(void);

/* LED 抽象层：逻辑 ID 设计，避免硬件细节泄露 */
typedef enum {
    BOARD_LED_1 = 0,
    BOARD_LED_2,
    BOARD_LED_3,
    BOARD_LED_4,
    BOARD_LED_MAX
} board_led_id_t;

typedef enum {
    LED_OFF = 0,
    LED_ON = 1
} led_state_t;

void board_led_init(void);
void board_led_set(board_led_id_t led, led_state_t state);
void board_led_toggle(board_led_id_t led);

/* 错误处理 */
void board_fatal_halt(void);

#endif // BOARD_H
