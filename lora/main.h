/*
 * lora::main for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _LORA_MAIN_HEADER_
#define _LORA_MAIN_HEADER_


#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#include "base64.h"
#include "led_utils.h"
#include "spi_utils.h"
#include "reg_bits.h"
#include "rfm9x.h"


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Usual header code here
 */

#define PIN_RESET               15
#define SEND_INTERVAL_MS        30000


#ifdef __cplusplus
}
#endif

#endif // _LORA_MAIN_HEADER_
