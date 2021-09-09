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
    spi_init(SPI_PORT, SPI_BAUDRATE);
    gpio_set_function(MISO_RX_GPIO, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_TX_GPIO, GPIO_FUNC_SPI);
    gpio_set_function(SCLK_GPIO, GPIO_FUNC_SPI);
    gpio_set_function(CSS_GPIO, GPIO_FUNC_SPI);
    gpio_pull_up(MISO_RX_GPIO);
    gpio_pull_up(MOSI_TX_GPIO);
    gpio_pull_up(SCLK_GPIO);
    gpio_pull_up(CSS_GPIO);
}

void read_into(uint8_t address, uint8_t* buffer, uint32_t length) {
    // Clear top bit to indicate a read
    cmd_buffer[0] = address & 0x7F;
    write(buffer, 1);
    read(buffer, length);
}

uint8_t read_u8(uint8_t address) {
    read_into(address, cmd_buffer, 1);
    return cmd_buffer[0];
}

void write_from(uint8_t address, uint8_t* buffer, uint32_t length) {
    // Set top bit to 1 to indicate a write
    cmd_buffer[0] = address | 0x80;
    write(cmd_buffer, 1);
    write(buffer, length);
}

uint32_t write_u8(uint8_t address, uint8_t byte) {
    // Set top bit to 1 to indicate a write
    cmd_buffer[0] = address | 0x80;
    cmd_buffer[1] = byte;
    return write(cmd_buffer, 2);
}

uint32_t read(uint8_t* buffer, uint32_t number_of_bytes) {
    return spi_read_blocking(SPI_PORT, 0x00, buffer, number_of_bytes);
}

uint32_t write(uint8_t* buffer, uint32_t number_of_bytes) {
    return spi_write_blocking(SPI_PORT, buffer, number_of_bytes);
}


}   // namespace SPI