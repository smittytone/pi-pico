/*
 * cellular::mcp9808_driver for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _MCP9808_HEADER_
#define _MCP9808_HEADER_


/*
 * PROTOTYPES
 */
// Default I2C address for device
#define MCP9808_I2CADDR_DEFAULT     0x18

// Register addresses
#define MCP9808_REG_CONFIG          0x01
#define MCP9808_REG_UPPER_TEMP      0x02
#define MCP9808_REG_LOWER_TEMP      0x03
#define MCP9808_REG_CRIT_TEMP       0x04
#define MCP9808_REG_AMBIENT_TEMP    0x05
#define MCP9808_REG_MANUF_ID        0x06
#define MCP9808_REG_DEVICE_ID       0x07


/**
    A very basic driver for the I2C-connected MCP9808 temperature sensor.
 */
class MCP9808 {

    public:
        // Constructor
        MCP9808(uint32_t i2c_address = MCP9808_I2CADDR_DEFAULT);

        bool        begin();
        double      read_temp();

    private:
        uint8_t     i2c_addr;
};


#endif // _MCP9808_HEADER_
