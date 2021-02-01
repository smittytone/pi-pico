#ifndef _WUMPUS_HEADER_
#define _WUMPUS_HEADER_

/*
 *  Includes
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"


/*
 *  Prototypes
 */
void setup();
void game_loop();

bool check_joystick(int x, int y);
uint8_t get_direction(int x, int y);

void setup();
void play_intro_theme();
void create_world();

void check_hazard();
void sense();
void draw_world();

void grabbed_by_bat();
void plunged_into_pit();

void fire_arrow();
void miss();
void kill_wumpus();
void wumpus_end();
void game_over();
void game_win();

void print_char(char c, int shift_speed);
void print_string(char *s, int shift_speed);

// I2C
void i2c_write_byte(uint8_t byte);
void i2c_write_block(uint8_t *data, uint8_t count);

// HT16K33
void ht16k33_init();
void ht16k33_power(uint8_t on);
void ht16k33_set_brightness(uint8_t brightness);
void ht16k33_draw_sprite(unsigned char *sprite);
void ht16k33_draw_sprite2(unsigned char *sprite);
void ht16k33_plot(uint8_t x, uint8_t y, bool set);
void ht16k33_print(char *text);
void ht16k33_clear();
void ht16k33_draw();

// Misc.
void delay(int time_in_ms);
int irandom(int start, int max);

// Stubs
void tone(uint8_t pin, unsigned int frequency, unsigned long duration);
bool digitalRead(uint8_t pin);

/*
 *      Constants
 */
#define I2C_PORT i2c0
#define I2C_FREQUENCY 400000
#define ON 1
#define OFF 0
#define SDA_GPIO 8
#define SCL_GPIO 9


/*
 *  Macros
 */
// Call this to reset the game.
void(* reset_func) (void) = 0;


#endif  // _WUMPUS_HEADER_
