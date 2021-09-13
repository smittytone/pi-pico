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
    sleep_ms(5000);
    #endif

    // Setup the LED
    LED::setup();

    // Setup SPI
    SPI::setup();

    // Initialise the radio
    RFM9x radio = RFM9x(PIN_RESET, 433.0);

    // Check if we are good to proceed
    if (radio.state) {
        #ifdef DEBUG
        printf("Radio good\n");
        #endif

        // Radio is good to use, apparently
        LED::on();
        uint32_t counter = 1;

        while (true) {
            std::string msg = "smittytone messes with micros msg # ";
            msg += std::to_string(counter);

            #ifdef DEBUG
            printf("MESSAGE: %s\n",msg.c_str());
            #endif

            msg = base64_encode(msg);

            #ifdef DEBUG
            printf("ENCODED: %s\n",msg.c_str());
            #endif

            bool success = radio.send((uint8_t*)msg.c_str(), msg.length(), false);

            if (success) {
                counter++;
            } else {
                printf("ERROR -- Send timeout\n");
            }

            sleep_ms(SEND_INTERVAL_MS);
        }
    } else {
        // ERROR -- blink the LED five times
        #ifdef DEBUG
        std::string err = std::to_string(radio.boot_err);
        printf("Radio bad: %s\n", err.c_str());
        #endif

        LED::blink(5);
        LED::off();
    }

    return 0;
}
