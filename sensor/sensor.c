/*
 *    sensor/sensor.c created by makepico
 */

#include "sensor.h"


/*
 * HT16K33 Segment Display Commands
 */
const int HT16K33_GENERIC_DISPLAY_ON = 0x81;
const int HT16K33_GENERIC_DISPLAY_OFF = 0x80;
const int HT16K33_GENERIC_SYSTEM_ON = 0x21;
const int HT16K33_GENERIC_SYSTEM_OFF = 0x20;
const int HT16K33_GENERIC_DISPLAY_ADDRESS = 0x00;
const int HT16K33_GENERIC_CMD_BRIGHTNESS = 0xE0;
const int HT16K33_GENERIC_CMD_BLINK = 0x81;
const int HT16K33_ADDRESS = 0x70;

// Segment character values
const int HT16K33_SEGMENT_COLON_ROW = 0x04;
const int HT16K33_SEGMENT_MINUS_CHAR = 0x10;
const int HT16K33_SEGMENT_DEGREE_CHAR = 0x11;
const int HT16K33_SEGMENT_SPACE_CHAR = 0x00;
const char CHARSET[] = "\x3F\x06\x5B\x4F\x66\x6D\x7D\x07\x7F\x6F\x5F\x7C\x58\x5E\x7B\x71\x40\x63";

// Position of each digit in the wider display buffer
const uint8_t POS[4] = {1, 3, 7, 9};

// The segment's display buffer
uint8_t buffer[17];

/*
 * SGP30 Commands
 */
const char SGP30_INIT_AIR_QUALITY[2] = "\x20\x03";
const char SGP30_MEASURE_AIR_QUALITY[2] = "\x20\x08";
const int SGP30_ADDRESS = 0x58;


/*
 * I2C Functions
 */

void i2c_write_byte(uint8_t address, uint8_t byte) {
    // Convenience function to write a single byte to the matrix
    i2c_write_blocking(I2C_PORT, address, &byte, 1, false);
}

void i2c_write_block(uint8_t address, uint8_t *data, uint8_t count) {
    // Convenience function to write a 'count' bytes to the matrix
    i2c_write_blocking(I2C_PORT, address, data, count, false);
}

void i2c_read_block(uint8_t address, uint8_t *data, uint8_t count) {
    i2c_read_blocking(I2C_PORT, address, data, count, false);
}


/*
 * HT16K33 LED Matrix Functions
 */
void ht16k33_init() {
    ht16k33_power(ON);
}

void ht16k33_power(uint8_t on) {
    // Turn the LED on or off
    i2c_write_byte(HT16K33_ADDRESS, on == ON ? HT16K33_GENERIC_SYSTEM_ON : HT16K33_GENERIC_DISPLAY_OFF);
    i2c_write_byte(HT16K33_ADDRESS, on == ON ? HT16K33_GENERIC_DISPLAY_ON : HT16K33_GENERIC_SYSTEM_OFF);
}

void ht16k33_bright(uint8_t brightness) {
    // Set the LED brightness
    if (brightness < 0 || brightness > 15) brightness = 15;
    i2c_write_byte(HT16K33_ADDRESS, HT16K33_GENERIC_CMD_BRIGHTNESS | brightness);
}

void ht16k33_set_glyph(uint8_t glyph, uint8_t digit, bool has_dot) {
    // Set an arbitrary segment pattern
    buffer[POS[digit]] = glyph;
    if (has_dot) buffer[POS[digit]] = buffer[POS[digit]] | 0x80;
}

void ht16k33_set_number(char number, uint8_t digit, bool has_dot) {
    // Display a numeric character at the specified display digit
    uint8_t char_val = 0xFF;

    if (number == '-') {
        char_val = HT16K33_SEGMENT_MINUS_CHAR;
    } else if (number == ' ') {
        char_val = HT16K33_SEGMENT_SPACE_CHAR;
    } else if (number >= 10 && number <= 15) {
        char_val = number;
    } else if (number >= 0 && number <= 9) {
        char_val = number;
    }

    // Bail on error...
    if (char_val == 0xFF) return;

    // ...otherwise put the glyph for the specified number
    // at the specified digit, turning on the decimal
    // point, if required
    buffer[POS[digit]] = CHARSET[char_val];
    if (has_dot) buffer[POS[digit]] = buffer[POS[digit]] | 0x80;
}

void ht16k33_clear() {
    // Clear the display buffer and then write it out
    for (uint8_t i = 0 ; i < 8 ; i++) buffer[i] = 0;
    ht16k33_draw();
}

void ht16k33_draw() {
    // Write out the transmit buffer to the LED
    i2c_write_block(HT16K33_ADDRESS, buffer, sizeof(buffer));
}


/*
 * SGP30 Sensor Functions
 */
void sgp30_init() {
    // Convenience function
    sgp30_start_measurement();
}

void sgp30_start_measurement() {
    // Send the initialization command
    uint8_t buffer[2];
    sgp30_set_data(buffer, SGP30_INIT_AIR_QUALITY, 0);
    i2c_write_block(SGP30_ADDRESS, buffer, sizeof(buffer));

    // Sleep for 15s as per Datasheet p8
    //sleep_ms(15000);
}

bool sgp30_get_measurement(uint8_t *reading_buffer) {
    // Get a single measurement
    uint8_t read_buffer[6];
    uint8_t cmd_buffer[2];

    // Request a reading
    sgp30_set_data(cmd_buffer, SGP30_MEASURE_AIR_QUALITY, 0);
    i2c_write_block(SGP30_ADDRESS, cmd_buffer, sizeof(cmd_buffer));

    // Wait a moment then read back the reading
    sleep_ms(25);
    i2c_read_block(SGP30_ADDRESS, read_buffer, sizeof(read_buffer));

    // Check the values
    uint8_t crc = read_buffer[2];
    uint16_t co2 = (read_buffer[0] << 8) | read_buffer[1];
    if (crc != sgp30_get_crc(co2)) {
        // Error!
        ht16k33_set_glyph(0x79, 0, false);
        ht16k33_set_glyph(0x50, 1, false);
        ht16k33_set_glyph(0x50, 2, false);
        ht16k33_set_glyph(0x00, 3, true);
        //ht16k33_draw();
        //return false;
    }

    // CO2
    reading_buffer[0] = read_buffer[0];
    reading_buffer[1] = read_buffer[1];

    // TVOC
    reading_buffer[2] = read_buffer[3];
    reading_buffer[3] = read_buffer[4];

    return true;
}

void sgp30_set_data(uint8_t *buffer, const char *data, uint8_t start) {
    // Write the data from the command constant string into the buffer
    for (uint8_t i = start ; i < sizeof(data) ; ++i) {
        buffer[i] = data[i];
    }
}

uint8_t sgp30_get_crc(uint16_t data) {
    // Calculate an 8-bit CRC from a 16-bit word
    // See section 6.6 of the SGP30 datasheet.
    uint8_t crc = 0xFF;
    crc = sub_crc(crc, ((data & 0xFF00) >> 8));
    crc = sub_crc(crc, (data & 0x00FF));
    return crc;
}

uint8_t sub_crc(uint8_t crc, uint8_t byte) {
    crc = crc ^ byte;
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0x31;
        } else {
            crc = crc << 1;
        }
    }
}

/*
 * Main Functions
 */

void bcd(uint16_t value, unsigned char* output_buffer) {
    // Convert the 16-bit 'value' to a Binary Coded Decimal
    // version, stored in 'output_buffer'
    // NOTE Can be refactored!
    *output_buffer = 0;
    while (value & 0xC000) {
        value -= 10000;
        *output_buffer += 1;
    }

    if (value >= 10000) {
        value -= 10000;
        *output_buffer += 1;
    }

    output_buffer++;
    *output_buffer = 0;
    while (value & 0x3C00) {
        value -= 1000;
        *output_buffer += 1;
    }

    if (value >= 1000) {
        value -= 1000;
        *output_buffer += 1;
    }

    output_buffer++;
    *output_buffer = 0;
    while (value & 0x0780) {
        value -= 100;
        *output_buffer += 1;
    }

    if (value >= 100) {
        value -= 100;
        *output_buffer += 1;
    }

    output_buffer++;
    *output_buffer = 0;
    while (value & 0x70) {
        value -= 10;
        *output_buffer += 1;
    }

    if (value >= 10) {
        value -= 10;
        *output_buffer += 1;
    }

    output_buffer++;
    *output_buffer = (unsigned char)value;
}

/*
 * Runtime start
 */

int main() {

    // Allocate storage for the sensor readings
    // and for the Binary Coded Decimal version of a
    // given reading (up to 5 digits)
    uint8_t reading_buffer[4];
    unsigned char decimal_reading[5];

    // Set up I2C
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_GPIO);
    gpio_pull_up(SCL_GPIO);

    // This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL)
    ht16k33_init();
    ht16k33_bright(2);

    // Zero the display
    for (uint8_t i = 0 ; i < 4 ; ++i)
        ht16k33_set_number(0, i, false);
    ht16k33_draw();

    // Initialize the sensor
    // This takes 15s to give a true reading, says the datasheet
    sgp30_init();

    // Loop
    while (1) {

        // Get a measurement and convert the CO2 value
        // to a 16-bit number -- range is 400-60,000 ppm
        if (sgp30_get_measurement(reading_buffer)) {
            uint16_t co2_reading = (reading_buffer[0] << 8) | reading_buffer[1];

            // Get the BCD version of the reading
            bcd(co2_reading, decimal_reading);

            // Display the value (lower four digits only)
            for (uint8_t i = 0 ; i < 4 ; ++i)
                ht16k33_set_number(decimal_reading[i + 1], i, false);
            ht16k33_draw();
        }

        // Sleep a bit
        sleep_ms(100);
    }

    // Just in case...
    return 0;
}