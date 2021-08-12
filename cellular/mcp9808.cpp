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

using std::string;

/**
    Instantiate a new MCP9808 object.
 */
MCP9808::MCP9808(uint32_t i2c_address) {
    if (i2c_address == 0x00 || i2c_address > 0xFF) {
        i2c_address = MCP9808_I2CADDR_DEFAULT;
    }

    i2c_addr = i2c_address;
}


/**
    Check the device is connected and operational.

    - Returns: `true` if the sensor is correct, otherwise `false`.
 */
bool MCP9808::begin() {
    uint8_t mid_data[2] = {0,0};
    uint8_t did_data[2] = {0,0};

    // Read bytes from the sensor: MID...
    i2c_write_byte(i2c_addr, MCP9808_REG_MANUF_ID);
    i2c_read_blocking(I2C_PORT, i2c_addr, mid_data, 2, false);

    // ...DID
    i2c_write_byte(i2c_addr, MCP9808_REG_DEVICE_ID);
    i2c_read_blocking(I2C_PORT, i2c_addr, did_data, 2, false);

    // Bytes to integers
    uint16_t mid_value = (mid_data[0] << 8) | mid_data[1];
    uint16_t did_value = (did_data[0] << 8) | did_data[1];

    // Returns True if the device is initialized, False otherwise.
    return (mid_value == 0x0054 && did_value == 0x0400);
}


/**
    Check the device is connected and operational.

    - Returns: `true` if the sensor is correct, otherwise `false`.
 */
double MCP9808::read_temp() {
    // Read sensor and return its value in degrees celsius.
    uint8_t temp_data[2] = {0,0};
    i2c_write_byte(i2c_addr, MCP9808_REG_AMBIENT_TEMP);
    i2c_read_blocking(I2C_PORT, i2c_addr, temp_data, 2, false);

    // Scale and convert to signed value.
    uint32_t temp_raw = (temp_data[0] << 8) | temp_data[1];
    double temp_cel = (temp_raw & 0x0FFF) / 16.0;
    if (temp_raw & 0x1000) temp_cel -= 256.0;
    return temp_cel;
}