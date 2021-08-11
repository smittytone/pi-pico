/*
 * cellular for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _CELLULAR_HEADER_
#define _CELLULAR_HEADER_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "modem.h"


/*
 * CONSTANTS
 */

#define PIN_MODEM_PWR           14
#define PIN_LED                 25

#define UART_BUFFER_SIZE        256
#define PIN_UART_TX             0
#define PIN_UART_RX             1
#define MODEM_UART              uart0

#define I2C_PORT                i2c1
#define I2C_FREQUENCY           400000
#define SDA_GPIO                2
#define SCL_GPIO                3

/*
 * PROTOTYPES
 */
void        setup();
void        setup_uart();
void        setup_led();
void        setup_modem_power_pin();

void        blink_led(uint32_t blinks);
void        led_on();
void        led_off();

void        setup_i2c();
void        i2c_write_byte(uint8_t address, uint8_t byte);
void        i2c_write_block(uint8_t address, uint8_t *data, uint8_t count);
void        i2c_read_block(uint8_t address, uint8_t *data, uint8_t count);


/*
 * GLOBALS
 */
extern char        uart_buffer[UART_BUFFER_SIZE];
extern char        *rx_ptr;


#endif // _CELLULAR_HEADER_
