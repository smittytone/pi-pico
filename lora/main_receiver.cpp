/*
 * lora::main::receiver for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "main.h"


int main() {
    #ifdef DEBUG
    stdio_init_all();
    #endif

    // Setup SPI
    SPI::setup();

    // Setup the LED
    LED::setup();

    // Initialise the radio
    RFM9x radio = RFM9x(PIN_RESET, 433.0);

    // Check if we are good to proceed
    if (radio.state) {
        // Radio is good to use, apparently
        radio.enable_crc(true);

        // Set the receiver node number
        radio.node = 0xFE;
    } else {
        // ERROR -- blink the LED five times
        LED::blink(5);
        LED::off();
    }

    return 0;
}
