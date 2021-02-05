/*
 *      sensor/sensor.h created by makepico
 */

#ifndef _I2C_HEADER_
#define _I2C_HEADER_

/*
 *      Includes
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

/*
 *      Prototypes
 */
void i2c_write_byte(uint8_t address, uint8_t byte);
void i2c_write_block(uint8_t address, uint8_t* data, uint8_t count);
void i2c_read_block(uint8_t address, uint8_t *data, uint8_t count);

void ht16k33_init(void);
void ht16k33_power(uint8_t value);
void ht16k33_bright(uint8_t value);
void ht16k33_set_glyph(uint8_t glyph, uint8_t digit, bool has_dot);
void ht16k33_set_number(char the_char, uint8_t digit, bool has_dot);
void ht16k33_clear();
void ht16k33_draw();

void sgp30_init();
void sgp30_start_measurement();
bool sgp30_get_measurement(uint8_t *reading_buffer);
void sgp30_set_data(uint8_t *buffer, const char *data, uint8_t start);
uint8_t sgp30_get_crc(uint16_t data);
uint8_t sub_crc(uint8_t crc, uint8_t byte);

void bcd(uint16_t value, unsigned char* output_buffer);

/*
 *      Constants
 */
#define I2C_PORT i2c0
#define I2C_FREQUENCY 400000
#define ON 1
#define OFF 0
#define SDA_GPIO 8
#define SCL_GPIO 9


#endif  // _I2C_HEADER_