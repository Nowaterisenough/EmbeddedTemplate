#include "main.h"

#include "board.h"
#include "board_config.h" // 包含板宏和 HAL 族头

int main(void)
{

    board_init();
    board_led_init();

    while (1) {
        // 如果板子实现了 LED，可直接使用
        board_led(BOARD_LED1_PIN, ON); // 亮
        board_delay_ms(1000);
        board_led(BOARD_LED1_PIN, OFF); // 灭
        board_delay_ms(1000);

        board_led(BOARD_LED2_PIN, ON); // 亮
        board_delay_ms(1000);
        board_led(BOARD_LED2_PIN, OFF); // 灭

        board_led(BOARD_LED3_PIN, ON); // 亮
        board_delay_ms(1000);
        board_led(BOARD_LED3_PIN, OFF); // 灭

        /*轮流显示 红绿蓝黄紫青白 颜色*/
        board_led(BOARD_LED1_PIN, ON);  // 亮
        board_led(BOARD_LED2_PIN, OFF); // 灭
        board_led(BOARD_LED3_PIN, OFF); // 灭
        board_delay_ms(1000);

        board_led(BOARD_LED1_PIN, OFF); // 亮
        board_led(BOARD_LED2_PIN, ON);  // 灭
        board_led(BOARD_LED3_PIN, OFF); // 灭
        board_delay_ms(1000);

        board_led(BOARD_LED1_PIN, OFF); // 亮
        board_led(BOARD_LED2_PIN, OFF); // 灭
        board_led(BOARD_LED3_PIN, ON);  // 灭
        board_delay_ms(1000);

        board_led(BOARD_LED1_PIN, ON);  // 亮
        board_led(BOARD_LED2_PIN, ON);  // 亮
        board_led(BOARD_LED3_PIN, OFF); // 灭
        board_delay_ms(1000);

        board_led(BOARD_LED1_PIN, ON);  // 亮
        board_led(BOARD_LED2_PIN, OFF); // 灭
        board_led(BOARD_LED3_PIN, ON);  // 亮
        board_delay_ms(1000);

        board_led(BOARD_LED1_PIN, OFF); // 灭
        board_led(BOARD_LED2_PIN, ON);  // 亮
        board_led(BOARD_LED3_PIN, ON);  // 亮
        board_delay_ms(1000);

        board_led(BOARD_LED1_PIN, ON); // 亮
        board_led(BOARD_LED2_PIN, ON); // 亮
        board_led(BOARD_LED3_PIN, ON); // 亮
        board_delay_ms(1000);

        board_led(BOARD_LED1_PIN, OFF); // 灭
        board_led(BOARD_LED2_PIN, OFF); // 灭
        board_led(BOARD_LED3_PIN, OFF); // 灭
        board_delay_ms(1000);
    }
}

void Error_Handler(void)
{
    board_fatal_halt();
}