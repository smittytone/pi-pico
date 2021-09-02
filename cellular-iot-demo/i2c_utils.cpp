/*
 * cellular::i2c_utils for Raspberry Pi Pico
 *
 * @version     1.0.1
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "cellular.h"


namespace I2C {

/**
    Setup the I2C block.

    Takes values from #defines set in i2c_utils.h
 */
void setup() {
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_GPIO);
    gpio_pull_up(SCL_GPIO);
}

/**
    Convenience function to write a single byte to the bus
 */
void write_byte(uint8_t address, uint8_t byte) {
    i2c_write_blocking(I2C_PORT, address, &byte, 1, false);
}

/**
    Convenience function to write a 'count' bytes to the bus
 */
void write_block(uint8_t address, uint8_t *data, uint8_t count) {
    i2c_write_blocking(I2C_PORT, address, data, count, false);
}

/**
    Convenience function to read 'count' bytes from the bus
 */
void read_block(uint8_t address, uint8_t *data, uint8_t count) {
    i2c_read_blocking(I2C_PORT, address, data, count, false);
}

}   // namespace I2C