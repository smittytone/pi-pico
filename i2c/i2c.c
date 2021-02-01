/*
 *    i2c/i2c.c created by makepico
 */

#include "i2c.h"


/*
 * HT16K33 LED Matrix Commands
 */
const int HT16K33_GENERIC_DISPLAY_ON = 0x81;
const int HT16K33_GENERIC_DISPLAY_OFF = 0x80;
const int HT16K33_GENERIC_SYSTEM_ON = 0x21;
const int HT16K33_GENERIC_SYSTEM_OFF = 0x20;
const int HT16K33_GENERIC_DISPLAY_ADDRESS = 0x00;
const int HT16K33_GENERIC_CMD_BRIGHTNESS = 0xE0;
const int HT16K33_GENERIC_CMD_BLINK = 0x81;
const int HT16K33_ADDRESS = 0x70;


/*
 * I2C Functions
 */

void i2c_write_byte(uint8_t byte) {
    // Convenience function to write a single byte to the matrix
    i2c_write_blocking(I2C_PORT, HT16K33_ADDRESS, &byte, 1, false);
}

void i2c_write_block(uint8_t *data, uint8_t count) {
    // Convenience function to write a 'count' bytes to the matrix
    i2c_write_blocking(I2C_PORT, HT16K33_ADDRESS, data, count, false);
}


/*
 * HT16K33 LED Matrix Functions
 */

void ht16k33_init() {
    ht16k33_power(ON);
}

void ht16k33_power(uint8_t on) {
    i2c_write_byte(on == ON ? HT16K33_GENERIC_SYSTEM_ON : HT16K33_GENERIC_DISPLAY_OFF);
    i2c_write_byte(on == ON ? HT16K33_GENERIC_DISPLAY_ON : HT16K33_GENERIC_SYSTEM_OFF);
}

void ht16k33_bright(uint8_t brightness) {
    // Set the LED brightness
    if (brightness < 0 || brightness > 15) brightness = 15;
    i2c_write_byte(HT16K33_GENERIC_CMD_BRIGHTNESS | brightness);
}

void ht16k33_clear(uint8_t *source_buffer) {
    // Clear the display buffer and then write it out
    for (uint8_t i = 0 ; i < 8 ; i++) source_buffer[i] = 0;
    ht16k33_draw(source_buffer);
}

void ht16k33_draw(uint8_t *source_buffer) {
    // Set up the buffer holding the data to be
    // transmitted to the LED
    uint8_t buffer[17];
    for (uint8_t i = 0 ; i < 17 ; i++) buffer[i] = 0;

    // Span the 8 bytes of the graphics buffer
    // across the 16 bytes of the LED's buffer
    for (uint8_t i = 0 ; i < 8 ; i++) {
        uint8_t a = source_buffer[i];
        buffer[i * 2 + 1] = (a >> 1) + ((a << 7) & 0xFF);
    }

    // Write out the transmit buffer
    i2c_write_block(buffer, sizeof(buffer));
}


/*
 * Main Functions
 */

void clear(uint8_t *display_buffer) {
    // Clear the display buffer
    for (uint8_t i = 0 ; i < 8 ; i++) display_buffer[i] = 0;
}


/*
 * Runtime start
 */

int main() {

    // Our data
    uint8_t buffer[9];
    uint8_t column_count = 0;
    uint8_t cycle_count = 0;
    uint8_t down = 0;

    // Set up I2C
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_GPIO);
    gpio_pull_up(SCL_GPIO);

    // This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL)
    ht16k33_init();
    ht16k33_bright(2);
    ht16k33_clear(buffer);

    // Initialize the first column
    buffer[0] = 1;

    // Initialize the PRNG
    srand(time(NULL));
    uint8_t r = rand() % 4 + 1;

    // Loop
    while (1) {
        // Draw the current buffer
        ht16k33_draw(buffer);

        // Update the buffer to bit shift each column
        if (down == 0) {
            for (uint8_t i = 0 ; i < 8 ; i++) {
                if (buffer[i] < 128) {
                    buffer[i] = buffer[i] << 1;
                } else {
                    buffer[i] = 1;
                    if (i == 0) cycle_count++;
                }
            }
        } else {
            for (uint8_t i = 0 ; i < 8 ; i++) {
                if (buffer[i] > 1) {
                    buffer[i] = buffer[i] >> 1;
                } else {
                    buffer[i] = 128;
                    if (i == 0) cycle_count++;
                }
            }
        }

        // Move the next start column on one
        column_count += 1;
        if (column_count > 7) column_count = 0;
        if (buffer[column_count] == 0) buffer[column_count] = 1;

        // Sleep a bit
        sleep_ms(25);

        if (cycle_count > r) {
            cycle_count = 0;
            down = down == 1 ? 0 : 1;
            r = rand() % 4 + 1;
        }
    }

    // Just in case...
    return 0;
}