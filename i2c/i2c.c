/*
 *    Project Flasher created by makepico
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

/*
   Connections on Raspberry Pi Pico board, other boards may vary.
   GPIO 8 (pin 11)-> SDA on LED Matrix
   GPIO 9 (pin 12)-> SCL on LED Matrix
   3.3v (pin 36) -> VCC on LED Matrix
   GND (pin 38)  -> GND on LED Matrix
*/

#define I2C_PORT i2c0
#define I2C_FREQUENCY 400000
#define ON 1
#define OFF 0
#define SDA_GPIO 8
#define SCL_GPIO 9


void i2c_write_byte(uint8_t);
void i2c_write_block(uint8_t*);
void ht16k33_power(uint8_t);
void ht16k33_init(void);
void ht16k33_draw(uint8_t*);
void ht16k33_clear(uint8_t*);
void clear(uint8_t*);
void ht16k33_bright(uint8_t);


// Commands
const int HT16K33_GENERIC_DISPLAY_ON = 0x81;
const int HT16K33_GENERIC_DISPLAY_OFF = 0x80;
const int HT16K33_GENERIC_SYSTEM_ON = 0x21;
const int HT16K33_GENERIC_SYSTEM_OFF = 0x20;
const int HT16K33_GENERIC_DISPLAY_ADDRESS = 0x00;
const int HT16K33_GENERIC_CMD_BRIGHTNESS = 0xE0;
const int HT16K33_GENERIC_CMD_BLINK = 0x81;
const int HT16K33_ADDRESS = 0x70;



void i2c_write_byte(uint8_t val) {
    i2c_write_blocking(I2C_PORT, HT16K33_ADDRESS, &val, 1, false);
}

void i2c_write_block(uint8_t *val) {
    i2c_write_blocking(I2C_PORT, HT16K33_ADDRESS, val, 17, false);
}

void ht16k33_power(uint8_t on) {
    i2c_write_byte(on == ON ? HT16K33_GENERIC_SYSTEM_ON : HT16K33_GENERIC_DISPLAY_OFF);
    i2c_write_byte(on == ON ? HT16K33_GENERIC_DISPLAY_ON : HT16K33_GENERIC_SYSTEM_OFF);
}

void ht16k33_init() {
    ht16k33_power(ON);
}

void ht16k33_draw(uint8_t *b) {
    // Set up the buffer holding the data to be
    // transmitted to the LED
    uint8_t buffer[17];
    for (uint8_t i = 0 ; i < 17 ; i++) buffer[i] = 0;

    // Span the 8 bytes of the graphics buffer
    // across the 16 bytes of the LED's buffer
    for (uint8_t i = 0 ; i < 8 ; i++) {
        uint8_t a = b[i];
        buffer[i * 2 + 1] = (a >> 1) + ((a << 7) & 0xFF);
    }

    // Write out the transmit buffer
    i2c_write_block(buffer);
}

void ht16k33_clear(uint8_t *b) {
    // Clear the display buffer and then write it out
    for (uint8_t i = 0 ; i < 8 ; i++) b[i] = 0;
    ht16k33_draw(b);
}

void ht16k33_bright(uint8_t b) {
    // Set the LED brightness
    if (b < 0 || b > 15) b = 15;
    i2c_write_byte(HT16K33_GENERIC_CMD_BRIGHTNESS | b);
}

void clear(uint8_t *b) {
    // Clear the display buffer
    for (uint8_t i = 0 ; i < 8 ; i++) b[i] = 0;
}



int main() {

    // Our data
    uint8_t buffer[9];
    uint8_t column_count = 0;

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

    // First line
    buffer[0] = 1;

    while (1) {
        // Draw the current buffer
        ht16k33_draw(buffer);

        // Update the buffer to bit shift each column
        for (uint8_t i = 0 ; i < 8 ; i++) {
            if (buffer[i] < 128) {
                buffer[i] = buffer[i] << 1;
            } else {
                buffer[i] = 1;
            }
        }

        // Move the next start column on one
        column_count += 1;
        if (column_count > 7) column_count = 0;
        if (buffer[column_count] == 0) buffer[column_count] = 1;

        // Sleep a bit
        sleep_ms(50);
    }

    return 0;
}
