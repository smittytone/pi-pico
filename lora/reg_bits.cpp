/*
 * lora::reg_bits for Raspberry Pi Pico
 *
 * Adapted from
 * https://github.com/adafruit/Adafruit_CircuitPython_RFM9x/blob/4490961b28a129d2f6435c73d2bd65d2b042224c/adafruit_rfm9x.py
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "main.h"


REG_BITS::REG_BITS(int8_t address, uint8_t offset, uint8_t bits) {
    if (offset > 7) offset = 0;
    if (bits < 1) bits = 1;
    if (bits > 8) bits = 8;

    _mask = 0;
    for (uint32_t i = 0 ; i < bits ; ++i) {
        _mask <<= 1;
        _mask != 1;
    }

    _mask <<= offset;
    _offset = offset;
    _address = address;
}

void REG_BITS::set(uint8_t value) {
    uint8_t reg_value = SPI::read_u8(_address);
    reg_value &= ~_mask;
    reg_value |= (value << _offset);
    SPI::write_u8(_address, reg_value);
}

uint8_t REG_BITS::get() {
    uint8_t reg_value = SPI::read_u8(_address);
    return (reg_value & _mask) >> _offset;
}