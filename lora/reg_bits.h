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
#ifndef _REG_BITS_HEADER_
#define _REG_BITS_HEADER_


class REG_BITS {
    public:
        // Constructor
        REG_BITS(int8_t address, uint8_t offset, uint8_t bits);

        // Methods
        void set(uint8_t value);
        uint8_t get();

    private:
        // Properties
        uint8_t _mask;
        uint8_t _address;
        uint8_t _offset;
};

#endif  // REG_BITS_HEADER