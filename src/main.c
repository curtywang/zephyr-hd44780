#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/gpio.h>
#include <inttypes.h>
// #include "button.h"
// #include "led.h"
#include "hd44780.h"
// #include "adc.h"

const uint8_t str1[] = "Hello from";
const uint8_t str2[] = CONFIG_BOARD;

void main(void)
{
    uint32_t i;
    uint8_t col = 0, btn = 0;
    printk("LCD Demo App starting!\n");

    // button_init();
    hd44780_init();
    //adc_init();

    while (1)
    {
        hd44780_cmd(HD44780_CMD_CLEAR, 0);

        hd44780_pos(0, col);
        for(i = 0; i < sizeof(str1) - 1; i++)
        {
            hd44780_data(str1[i]);
        }

        hd44780_pos(1, col);
        for(i = 0; i < sizeof(str2) - 1; i++)
        {
            hd44780_data(str2[i]);
        }
        col++;
        if (col > 3)
            col = 0;

        hd44780_pos(0, 15);
        // //btn = adc_sample();
        // switch (btn)
        // {
        //     case btn_none:
        //         hd44780_data(0xBC); // sort of smile, ROM Code: A00
        //         break;

        //     case btn_select:
        //         hd44780_data(0xF4); // omega, ROM Code: A00
        //         break;

        //     case btn_left:
        //         hd44780_data(0x7F); // left arrow, ROM Code: A00
        //         break;

        //     case btn_down:
        //         hd44780_data('v');
        //         break;

        //     case btn_up:
        //         hd44780_data('^');
        //         break;

        //     case btn_right:
        //         hd44780_data(0x7E); // right arrow, ROM Code: A00
        //         break;

        //     default:
        //         hd44780_data('E');
        //         break;
        // }

        k_sleep(K_MSEC(1000));
    }
}

