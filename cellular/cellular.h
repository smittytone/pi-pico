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

#define PIN_UART_TX             0
#define PIN_UART_RX             1

#define PIN_MODEM_PWR           14

#define PIN_LED                 25


/*
 * PROTOTYPES
 */
void    setup();
void    setup_uart();
void    setup_led();
void    setup_modem_power_pin();

void    blink_led(uint32_t blinks);


#endif // _CELLULAR_HEADER_
