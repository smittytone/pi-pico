/*
 * lora::reg_bits for Raspberry Pi Pico
 *
 *
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "main.h"


/*
    This code creates a mask for the specied number of bits,
    and adjusts it along the bit sequence by the specified offset. This is
    very handy for SX127 registers containing multiple values.

    See `rfm9x.h` for examples of its use.

    Adapted from:
    https://github.com/adafruit/Adafruit_CircuitPython_RFM9x/blob/main/adafruit_rfm9x.py
 */
REG_BITS::REG_BITS(int8_t address, uint8_t offset, uint8_t bits) {
    if (offset > 7) offset = 0;
    if (bits < 1) bits = 1;
    if (bits > 8) bits = 8;

    // Build the mask
    _mask = 0xFF >> (8 - bits);

    // Slide the mask along and
    // store the input values
    _mask <<= offset;
    _offset = offset;
    _address = address;
}

void REG_BITS::set(uint8_t value) {
    uint8_t reg_value = SPI::read_byte(_address);
    reg_value &= ~_mask;
    reg_value |= (value << _offset);
    SPI::write_byte(_address, reg_value);
}

uint8_t REG_BITS::get() {
    uint8_t reg_value = SPI::read_byte(_address);
    return (reg_value & _mask) >> _offset;
}