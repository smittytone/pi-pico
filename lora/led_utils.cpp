/*
 * lora::led_utils for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "main.h"


namespace LED {


void setup() {
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    off();
}

void on() {
    gpio_put(PIN_LED, true);
}

void off() {
    gpio_put(PIN_LED, false);
}

/**
    Blink the Pico LED a specified number of times, leaving it
    on at the end.
    - Parameters:
        - blinks: The number of flashes.
 */
void blink(uint32_t blinks) {
    for (uint32_t i = 0 ; i < blinks ; ++i) {
        off();
        sleep_ms(250);
        on();
        sleep_ms(250);
    }
}


}   // namespace LED