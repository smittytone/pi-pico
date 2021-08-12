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


/*
 * GLOBALS
 */



int main() {

    // Set up the hardware
    setup();

    // Instantiate the modem
    Sim7080G modem;

    // Fire up the modem
    if (modem.init_modem()) {
        // Light the LED
        led_on();

        // Start to listen for commands
        // listen();
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
    setup_led();
    setup_i2c();
    setup_uart();
    setup_modem_power_pin();
}


/**
    Set up the UART link to the modem.
 */
void setup_uart() {
    // Initialise UART 0
    uart_init(MODEM_UART, 115200);

    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(PIN_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_UART_RX, GPIO_FUNC_UART);

    // Clear the UART processing buffer
    //clear_buffer();
}


/*
 * LED Functions
 */
void setup_led() {
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    led_off();
}

void led_on() {
    gpio_put(PIN_LED, true);
}

void led_off() {
    gpio_put(PIN_LED, false);
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


/*
 * Modem PWR_EN Functions
 */
void setup_modem_power_pin() {
    gpio_init(PIN_MODEM_PWR);
    gpio_set_dir(PIN_MODEM_PWR, GPIO_OUT);
    gpio_put(PIN_MODEM_PWR, false);
}


/*
 * I2C Functions
 */
void setup_i2c() {
    // Initialize the I2C bus for the display and sensor
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_GPIO);
    gpio_pull_up(SCL_GPIO);

    // TODO
    // Initialize the display
    // Initialize the sensor
}

void i2c_write_byte(uint8_t address, uint8_t byte) {
    // Convenience function to write a single byte to the bus
    i2c_write_blocking(I2C_PORT, address, &byte, 1, false);
}

void i2c_write_block(uint8_t address, uint8_t *data, uint8_t count) {
    // Convenience function to write a 'count' bytes to the bus
    i2c_write_blocking(I2C_PORT, address, data, count, false);
}

void i2c_read_block(uint8_t address, uint8_t *data, uint8_t count) {
    // Convenience function to read 'count' bytes from the bus
    i2c_read_blocking(I2C_PORT, address, data, count, false);
}