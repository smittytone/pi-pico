/*
 * lora::spi_utils for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "main.h"


uint8_t cmd_buffer[4] = {0,0,0,0};


namespace SPI {


void setup() {
    // Set SPI baudrate to 5MHz to avoid problems, but can go to 10MHz
    spi_init(SPI_PORT, SPI_BAUDRATE);
    gpio_set_function(MISO_RX_GPIO, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_TX_GPIO, GPIO_FUNC_SPI);
    gpio_set_function(SCLK_GPIO, GPIO_FUNC_SPI);

    // Set up Chip Select (NSS) pin via GPIO
    gpio_init(CSS_GPIO);
    gpio_set_dir(CSS_GPIO, GPIO_OUT);
    gpio_put(CSS_GPIO, 0);
}

/*
    Read in a single byte
 */
uint8_t read_byte(uint8_t address) {
    // Read into the command buffer for convenience
    read_into(address, cmd_buffer, 1);
    return cmd_buffer[0];
}

/*
    Read a number of bytes into the specified buffer.
 */
void read_into(uint8_t address, uint8_t* read_buffer, uint32_t length) {
    // Clear top bit to indicate a read
    cmd_buffer[0] = address & 0x7F;

    // Start the transaction
    cs_select();

    // Send the address
    write(cmd_buffer, 1);
    sleep_ms(10);

    // Read back the value
    read(read_buffer, length);

    // End the transaction
    cs_select(false);
    sleep_ms(10);
}

/*
    Write out a single byte
 */
void write_byte(uint8_t address, uint8_t byte) {
    // Write from the command buffer for convenience
    cmd_buffer[1] = byte;

    // Set top bit to 1 to indicate a write
    cmd_buffer[0] = address | 0x80;

    // Start the transaction
    cs_select();

    // Send address and byte as a single transaction
    write(cmd_buffer, 2);

    // End the transaction
    cs_select(false);
    sleep_ms(10);
    return;
}

/*
    Write out a number of bytes from the specified buffer.
 */
uint32_t write_from(uint8_t address, uint8_t* write_buffer, uint32_t length) {
    // Set top bit to 1 to indicate a write
    cmd_buffer[0] = address | 0x80;

    // Start the transaction
    cs_select();

    // Send the address
    write(cmd_buffer, 1);
    sleep_ms(10);

    // Send the data
    uint32_t bytes_written = write(write_buffer, length);

    // End the transaction
    cs_select(false);
    sleep_ms(10);
    return bytes_written;
}

/*
    Generic SPI read function: bytes into the specified buffer.
 */
uint32_t read(uint8_t* buffer, uint32_t number_of_bytes) {
    uint32_t bytes_read = spi_read_blocking(SPI_PORT, 0x00, buffer, number_of_bytes);
    return bytes_read;
}

/*
    Generic SPI write function: bytes out from specified buffer.
 */
uint32_t write(uint8_t* buffer, uint32_t number_of_bytes) {
    uint32_t bytes_written = spi_write_blocking(SPI_PORT, buffer, number_of_bytes);
    return bytes_written;
}

void cs_select(bool state) {
    gpio_put(CSS_GPIO, !state);
}


}   // namespace SPI