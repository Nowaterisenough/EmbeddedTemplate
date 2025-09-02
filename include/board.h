#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/* 统一板级接口：跨板保持一致，不包含芯片族头/板宏 */
void     board_init(void);
void     board_delay_ms(uint32_t ms);
uint32_t board_millis(void);

/* 可选：LED 封装（若某板未实现，可留空实现或用 BOARD_HAS_LED 宏控制调用处） */
void board_led_init(void);
void board_led(uint16_t led, uint8_t state);
void board_fatal_halt(void);

#endif // BOARD_H
