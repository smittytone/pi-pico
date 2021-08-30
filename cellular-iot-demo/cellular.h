/*
 * cellular::main for Raspberry Pi Pico
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
#include <sstream>
#include <iomanip>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"

#include "modem.h"
#include "mcp9808.h"
#include "utils.h"
#include "i2c_utils.h"
#include "ht16k33.h"
#include "base64.h"
#include "ArduinoJson-v6.18.3.h"


#ifdef __cplusplus
extern "C" {
#endif


/*
 * CONSTANTS
 */
#define PIN_MODEM_PWR           14
#define PIN_LED                 25

#define ERR_CODE_GEN_FAIL       "LBBLBBLBBLBBL"
#define ERR_CODE_MODEM_START    "LBS"
#define ERR_CODE_MODEM_INIT     "LBSBS"


/*
 * PROTOTYPES
 */
void        setup();
void        setup_i2c();
void        setup_uart();
void        setup_led();
void        setup_modem_power_pin();

void        led_on();
void        led_off();
void        blink_led(uint32_t blinks);
void        blink_err_code(std::string code);

void        listen();
void        process_command_led(uint32_t blinks);
void        process_command_num(uint32_t number);
void        process_command_tmp();
void        process_command_get();
void        process_command_flash(std::string code);


#ifdef __cplusplus
}           // extern "C"
#endif


#endif      // _CELLULAR_HEADER_
