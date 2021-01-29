/*
 *      i2c/i2c.h created by makepico
 */

#ifndef _I2C_HEADER_
#define _I2C_HEADER_

/*
 *      Includes
 */
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"


/*
 *      Prototypes
 */
void i2c_write_byte(uint8_t);
void i2c_write_block(uint8_t*, uint8_t);
void ht16k33_power(uint8_t);
void ht16k33_init(void);
void ht16k33_draw(uint8_t*);
void ht16k33_clear(uint8_t*);
void clear(uint8_t*);
void ht16k33_bright(uint8_t);


/*
 *      Constants
 */
#define I2C_PORT i2c0
#define I2C_FREQUENCY 400000
#define ON 1
#define OFF 0
#define SDA_GPIO 8
#define SCL_GPIO 9


#endif  // _I2C_HEADER_