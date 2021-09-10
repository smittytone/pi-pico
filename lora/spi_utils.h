/*
 * lora::spi_utils for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _SPI_HEADER_
#define _SPI_HEADER_


/*
 * CONSTANTS
 */
#define SPI_PORT                spi0
#define SPI_BAUDRATE            5000000
#define MISO_RX_GPIO            16
#define MOSI_TX_GPIO            19
#define SCLK_GPIO               18
#define CSS_GPIO                17


/*
 * PROTOTYPES
 */
namespace SPI {
    void            setup();
    uint8_t         read_byte(uint8_t address);
    void            read_into(uint8_t address, uint8_t* buffer, uint32_t length);
    uint32_t        read(uint8_t* buffer, uint32_t number_of_bytes);
    void            write_byte(uint8_t address, uint8_t byte);
    uint32_t        write_from(uint8_t address, uint8_t* buffer, uint32_t length);
    uint32_t        write(uint8_t* buffer, uint32_t number_of_bytes);
    void            cs_select(bool state = true);
}


#endif  // _SPI_HEADER_