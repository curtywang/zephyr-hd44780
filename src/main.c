#include <zephyr.h>
#include <sys/printk.h>
#include <inttypes.h>
// #include "button.h"
// #include "led.h"
#include <hd44780.h>
// #include "adc.h"

const char str1[] = "Hello from";
const char str2[] = CONFIG_BOARD;


void write_to_lcd(char* input_str, size_t input_str_len) {
    for(size_t i = 0; i < 16; i++) {
        if (i >= input_str_len) {
            hd44780_data(' ');
        }
        else {
            hd44780_data(input_str[i]);
        }
    }
}


void main(void)
{
    printk("LCD Demo App starting!\n");

    hd44780_init();
    hd44780_cmd(HD44780_CMD_CLEAR, 0);

    while (true)
    {
        hd44780_pos(0, 0);
        write_to_lcd(str1, sizeof(str1) - 1);

        hd44780_pos(1, 0);
        write_to_lcd(str2, sizeof(str2) - 1);

        k_sleep(K_MSEC(1000));
        
        hd44780_pos(0, 0);
        for(size_t i = 0; i < 16; i++)
        {
            hd44780_data('O');
        }

        hd44780_pos(1, 0);
        for(size_t i = 0; i < 16; i++)
        {
            hd44780_data('O');
        }

        k_sleep(K_MSEC(1000));
        
    }
}

