#include <zephyr.h>
#include <devicetree.h>
#include <stdlib.h>
#include <sys/printk.h>
#include <inttypes.h>
#include <hd44780.h>

// setup network and mqtt client
#include <wifi_setup.h>
#include <mqtt_setup.h>


const char str1[] = "Hello from";
const char str2[] = CONFIG_BOARD;


void write_to_lcd(const char* input_str, size_t input_str_len) {
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
    // uint8_t msg_to_publish[40];
    // initialize_network();
    // mqtt_client_setup_and_connect();

    printk("LCD Demo App starting!\n");

    hd44780_init();
    hd44780_cmd(HD44780_CMD_CLEAR, 0);

    while (true)
    {
        hd44780_pos(0, 0);
        write_to_lcd(str1, sizeof(str1) - 1);

        hd44780_pos(1, 0);
        write_to_lcd(str2, sizeof(str2) - 1);

        // snprintf(msg_to_publish, 40, "hello! %d", k_uptime_get_32());
        // mqtt_client_publish_message(msg_to_publish, 40);
        k_sleep(K_MSEC(500));
        
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

        mqtt_client_process_keepalive();
        k_sleep(K_MSEC(500));
        
    }
}

