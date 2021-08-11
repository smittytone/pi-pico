/*
 * cellular for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "cellular.h"


int main() {

    // Set up the hardware
    setup();

    // Fire up the modem
    if (init_modem()) {
        // Start to listen for commands
    } else {
        // Flash the LED five times, turn it off and exit
        blink_led(5);
        gpio_put(PIN_LED, false);
    }

    return 0;
}


/**
    Umbrella setup routine.
 */
void setup() {
    setup_uart();
    setup_led();
    setup_modem_power_pin();
}


/**
    Set up the UART link to the modem.
 */
void setup_uart() {
    // Initialise UART 0
    uart_init(uart0, 115200);

    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(PIN_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_UART_RX, GPIO_FUNC_UART);
}


/**
    Set up the Pico's LED.
 */
void setup_led() {
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    gpio_put(PIN_LED, false);
}


/**
    Set up the pin the toggles the modem power.
    See `toggle_module_power()` in `modem.c`.
 */
void setup_modem_power_pin() {
    gpio_init(PIN_MODEM_PWR);
    gpio_set_dir(PIN_MODEM_PWR, GPIO_OUT);
    gpio_put(PIN_MODEM_PWR, false);
}


/**
    Blink the Pico LED a specified number of times, leaving it
    on at the end.

    - Parameters:
        - blinks: The number of flashes.
 */
void blink_led(uint32_t blinks) {
    for (uint32_t i = 0 ; i < blinks ; ++i) {
        gpio_put(PIN_LED, false);
        sleep_ms(250);
        gpio_put(PIN_LED, true);
        sleep_ms(250);
    }
}


// strtol( const char *restrict str, char **restrict str_end, int base );