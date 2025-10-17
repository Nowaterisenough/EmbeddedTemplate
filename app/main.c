#include "main.h"
#include "board.h"

/*
 * LED 闪烁序列（数据驱动设计）
 * 每个元素是 3 位 RGB 掩码：bit0=LED1, bit1=LED2, bit2=LED3
 *
 * 序列说明：
 * - 单色循环：红 -> 绿 -> 蓝
 * - 混合颜色：黄(R+G) -> 紫(R+B) -> 青(G+B) -> 白(R+G+B)
 * - 全灭
 */
static const uint8_t led_sequence[] = {
    0b001,  /* Red (LED1) */
    0b010,  /* Green (LED2) */
    0b100,  /* Blue (LED3) */
    0b011,  /* Yellow (LED1 + LED2) */
    0b101,  /* Magenta (LED1 + LED3) */
    0b110,  /* Cyan (LED2 + LED3) */
    0b111,  /* White (LED1 + LED2 + LED3) */
    0b000,  /* All OFF */
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define LED_DELAY_MS    1000

int main(void)
{
    board_init();
    board_led_init();

    while (1) {
        for (size_t i = 0; i < ARRAY_SIZE(led_sequence); i++) {
            uint8_t pattern = led_sequence[i];

            /* 根据位掩码设置 LED 状态 */
            board_led_set(BOARD_LED_1, (pattern & 0x1) ? LED_ON : LED_OFF);
            board_led_set(BOARD_LED_2, (pattern & 0x2) ? LED_ON : LED_OFF);
            board_led_set(BOARD_LED_3, (pattern & 0x4) ? LED_ON : LED_OFF);

            board_delay_ms(LED_DELAY_MS);
        }
    }
}

void Error_Handler(void)
{
    board_fatal_halt();
}
