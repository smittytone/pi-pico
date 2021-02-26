/*
 * 'Hunt the Wumpus' for Raspberry Pi Pico
 *
 * By Tony Smith
 * Original version by Corey Faure
 *
 * Version 1.0.2
 *
 */
#ifndef _WUMPUS_HEADER_
#define _WUMPUS_HEADER_

/*
 *  Includes
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Sprite Data
#include "sprites.h"


/*
 *  Function Prototypes
 */
// Game init
void setup();
void create_world();
void play_intro();

// Game run
void game_loop();
bool check_joystick(uint16_t x, uint16_t y);
uint8_t get_direction(uint16_t x, uint16_t y);

// Environment
void draw_world();
void check_senses();
bool check_hazards();
void clear_pins();

// Player events
void grabbed_by_bat();
void plunged_into_pit();

// Wumpus attack animations
void fire_arrow_animation();
void arrow_miss_animation();
void dead_wumpus_animation();
void wumpus_win_animation();

// Game outcomes
void game_won();
void game_lost();
void game_over(const char *text);

// I2C
void i2c_write_byte(uint8_t byte);
void i2c_write_block(uint8_t *data, uint8_t count);

// HT16K33
void ht16k33_init();
void ht16k33_power_on_or_off(uint8_t on);
void ht16k33_set_brightness(uint8_t brightness);
void ht16k33_draw_sprite(const char *sprite);
void ht16k33_draw_sprite_center(const char *sprite);
void ht16k33_plot(uint8_t x, uint8_t y, bool set);
void ht16k33_print(const char *text);
void ht16k33_clear();
void ht16k33_draw();

// Misc.
int irandom(int start, int max);

// Stubs
void tone(unsigned int frequency, unsigned long duration, unsigned long post) ;
bool digitalRead(uint8_t pin);

/*
 *  Constants
 */
#define I2C_PORT i2c0
#define I2C_FREQUENCY   400000

#define ON              1
#define OFF             0

#define PIN_SDA         8
#define PIN_SCL         9

#define PIN_GREEN       20
#define PIN_RED         21
#define PIN_SPEAKER     16
#define PIN_Y           27
#define PIN_X           26
#define PIN_BUTTON      19

#define DEADZONE        400
#define UPPER_LIMIT     2448
#define LOWER_LIMIT     1648

// HT16K33 LED Matrix Commands
#define HT16K33_GENERIC_DISPLAY_ON      0x81
#define HT16K33_GENERIC_DISPLAY_OFF     0x80
#define HT16K33_GENERIC_SYSTEM_ON       0x21
#define HT16K33_GENERIC_SYSTEM_OFF      0x20
#define HT16K33_GENERIC_DISPLAY_ADDRESS 0x00
#define HT16K33_GENERIC_CMD_BRIGHTNESS  0xE0
#define HT16K33_GENERIC_CMD_BLINK       0x81
#define HT16K33_ADDRESS                 0x70

#define DEBOUNCE_TIME_US                10000

// _WUMPUS_HEADER_
#endif
