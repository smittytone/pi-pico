#ifndef _PHANTOMS_MAIN_HEADER_
#define _PHANTOMS_MAIN_HEADER_


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

// Game headers
#include "map.h"
#include "ssd1306.h"
#include "sprites.h"


/*
 * STRUCTURES
 */
typedef struct {
    uint8_t origin_x;
    uint8_t origin_y;
    uint8_t width;
    uint8_t height;
} Rect;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t hp;
} Phantom;

typedef struct {
    bool in_play;
    bool show_reticule;
    bool is_firing;
    bool can_teleport;
    bool is_joystick_centred;

    uint8_t phantoms;
    uint8_t audio_range;
    uint8_t tele_x;
    uint8_t tele_y;

    uint16_t level;
    uint16_t score;
    uint32_t zap_time;
    uint32_t debounce_count_button;
} Game;


/*
 * PROTOTYPES
 */
void setup();
void create_world();
void init_game();
void game_loop();
void update_world(uint32_t now);

bool check_joystick(uint16_t x, uint16_t y) ;
uint8_t get_direction(uint16_t x, uint16_t y);

bool check_hazard(uint8_t x, uint8_t y);
void check_senses();
void do_teleport();
void fire_laser();
void death();

void draw_screen();
void draw_floor_line(uint8_t inset);
void draw_teleporter(uint8_t inset);
void draw_left_wall(uint8_t steps, bool left_open);
void draw_right_wall(uint8_t steps, bool right_open);
void draw_end(uint8_t steps);
void draw_dir_arrow();
void draw_phantom(uint8_t x, uint8_t y, uint8_t c);
void move_phantoms();

uint8_t get_facing_phantom(uint8_t range);
uint8_t locate_phantom(uint8_t x, uint8_t y);

int irandom(int start, int max);
void tone(unsigned int frequency, unsigned long duration, unsigned long post);

/*
 *  CONSTANTS
 */
#define I2C_PORT                                        i2c0
#define I2C_FREQUENCY                                   400000

#define ON                                              true
#define OFF                                             false

#define PIN_SDA                                         8
#define PIN_SCL                                         9
#define SSD1306_RST_PIN                                 16
#define PIN_SPEAKER                                     4
#define PIN_TELE_BUTTON                                 18
#define PIN_FIRE_BUTTON                                 17
#define PIN_LED                                         25
#define PIN_X                                           26
#define PIN_Y                                           27

// Player movement directions
#define DIRECTION_NORTH                                 0
#define DIRECTION_EAST                                  1
#define DIRECTION_SOUTH                                 2
#define DIRECTION_WEST                                  3

#define MOVE_FORWARD                                    0
#define TURN_RIGHT                                      1
#define MOVE_BACKWARD                                   2
#define TURN_LEFT                                       3

#define DEADZONE                                        400
#define UPPER_LIMIT                                     2448
#define LOWER_LIMIT                                     1648
#define JOY_MAX                                         4096

#define DEBOUNCE_TIME_US                                10000
#define ANIM_TIME_US                                    22000
#define PHANTOM_MOVE_TIME_US                            1000000

// Map square types
#define MAP_TILE_CLEAR                                  0xFF
#define MAP_TILE_WALL                                   0x00
#define MAP_TILE_TELEPORTER                             0xAA


/*
 *  GLOBALS
 */
uint8_t  oled_height;
uint8_t  oled_width;
uint8_t  oled_i2c_addr;
bool     oled_inverted;

// Graphics buffer
uint8_t  oled_buffer[1024];
uint16_t oled_buffer_size;
uint8_t  i2c_tx_buffer[1025];
uint16_t i2c_tx_buffer_size;

// Player
uint8_t player_x;
uint8_t player_y;
uint8_t player_direction;

// Graphics structures
Rect rects[7];

// Game data
Phantom phantoms[3];
Game game;

uint32_t last_draw;
uint32_t last_phantom_move;

// _PHANTOMS_MAIN_HEADER_
#endif
