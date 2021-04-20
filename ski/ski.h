/*
 * ski for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _SKI_MAIN_HEADER_
#define _SKI_MAIN_HEADER_


#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

using std::string;
using std::vector;

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

#include "ssd1306.h"
#include "tinymt32.h"
#include "utils.h"


/*
 *  CONSTANTS
 */
#define ON                                              true
#define OFF                                             false

#define PIN_SDA                                         8
#define PIN_SCL                                         9
#define SSD1306_RST_PIN                                 19
#define PIN_SPEAKER                                     20
#define PIN_TURN_BUTTON                                 16
#define PIN_PAUSE_BUTTON                                17
#define PIN_LED                                         25
#define PIN_X                                           26
#define PIN_Y                                           27

// I2C
#define I2C_PORT                                        i2c0
#define I2C_FREQUENCY                                   400000

#define SCREEN_WIDTH                                    128
#define SCREEN_HEIGHT                                   64
#define DEBOUNCE_TIME_US                                20000
#define PLAYER_Y                                        12


// This comes AFTER the #includes
#ifdef __cplusplus
    extern "C" {
#endif


typedef struct {
    uint8_t     player_x;
    int8_t      delta_x;
    uint8_t     level;
    uint16_t    metres;
    int8_t      speed;
    bool        ghost;
    bool        paused;
} Game;


/*
 * PROTOTYPES
 */
void        setup();
Game        init_game();
void        game_loop(SSD1306 &display);
void        draw_tree(uint8_t x, SSD1306 &display);
void        draw_prize(uint8_t x, SSD1306 &display);
void        crash();
bool        play_again();


/*
 * GLOBALS
 */
extern tinymt32_t   tinymt_store;


// extern "C"
}

#endif  // _SKI_MAIN_HEADER_