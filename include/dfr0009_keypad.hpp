#ifndef DFR0009_KEYPAD_H
#define DFR0009_KEYPAD_H

#include <zephyr.h>
#include <drivers/adc.h>

class DFR0009Keypad {
    public:
    const struct device* _adc_pin;
    int16_t sample_buffer;
    struct adc_sequence _adc_seq = {
        .options = NULL,
        .channels = BIT(1)
    };
    enum KeyPressed {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        NONE
    };

    DFR0009Keypad(const struct device* in_adc_pin) {
        _adc_pin = in_adc_pin;

    };



    KeyPressed get_key_pressed() {

    }

};


#endif // DFR0009_KEYPAD_H
