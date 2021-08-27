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
#include <algorithm>

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

using std::vector;
using std::string;


#ifdef __cplusplus
extern "C" {
#endif


/*
 * CONSTANTS
 */
#define PIN_MODEM_PWR           14
#define PIN_LED                 25

#define ERR_CODE_NO_MODEM       "SBSBSBSBS"
#define ERR_CODE_MODEM_START    "LBSB"
#define ERR_CODE_MODEM_INIT     "LBSBSB"


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
void        blink_err_code(string code);

void        listen();
void        process_command_led(uint32_t blinks);
void        process_command_num(uint32_t number);
void        process_command_tmp();
void        process_command_get();
void        process_command_post(string data);
void        process_command_flash(string code);

void        process_request(string server, string path, string data = "");


}   // extern "C"

#endif // _CELLULAR_HEADER_
