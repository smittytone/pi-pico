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

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"

#include "modem.h"
#include "mcp9808.h"


#ifdef __cplusplus
extern "C" {
#endif


/*
 * CONSTANTS
 */
#define PIN_MODEM_PWR           14
#define PIN_LED                 25

#define I2C_PORT                i2c1
#define I2C_FREQUENCY           400000
#define SDA_GPIO                2
#define SCL_GPIO                3

#define ERR_CODE_GEN_FAIL       "LBBLBBLBBLBBL"
#define ERR_CODE_MODEM_START    "LBS"
#define ERR_CODE_MODEM_INIT     "LBSBS"

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
void        blink_err_code(string code);

void        setup_i2c();
void        i2c_write_byte(uint8_t address, uint8_t byte);
void        i2c_write_block(uint8_t address, uint8_t *data, uint8_t count);
void        i2c_read_block(uint8_t address, uint8_t *data, uint8_t count);

void        listen();
void        process_command_led(string msg);
void        process_command_tmp();

std::vector<string> split_to_lines(string str);

// extern "C"
}

#endif // _CELLULAR_HEADER_
