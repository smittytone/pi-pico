/*
 * lora::main::sender for Raspberry Pi Pico
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

        // Set the sender node number
        radio.node = 0xFA;

        // Set the receiver node number
        radio.destination = 0xFE;

        uint32_t counter = 1;

        while (true) {
            std::string msg = "smittytone messes with micros msg # ";
            msg += std::to_string(counter);

            #ifdef DEBUG
            printf(msg.c_str());
            #endif

            msg = base64_encode(msg);

            #ifdef DEBUG
            printf(msg.c_str());
            #endif

            radio.send((uint8_t*)msg.c_str(), msg.length() - 1);
            counter++;
            sleep_ms(SEND_INTERVAL_MS);
        }
    } else {
        // ERROR -- blink the LED five times
        LED::blink(5);
        LED::off();
    }

    return 0;
}
