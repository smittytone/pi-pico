/*
 * Planet Invasion for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _PLANETI_HEADER_
#define _PLANETI_HEADER_


/*
 * INCLUDES
 */
// Standard, Pico includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

// Game includes
#include "entities.h"
#include "invaders.h"
#include "ssd1306.h"
#include "tinymt32.h"
#include "utils.h"


/*
 *  CONSTANTS
 */
#define I2C_PORT                                        i2c0
#define I2C_FREQUENCY                                   400000

#define ON                                              true
#define OFF                                             false

#define PIN_SDA                                         8
#define PIN_SCL                                         9
#define PIN_FIRE_BUTTON                                 16
#define PIN_BOMB_BUTTON                                 17
#define SSD1306_RST_PIN                                 18
#define PIN_SPEAKER                                     20
#define PIN_LED                                         25
#define PIN_X                                           26
#define PIN_Y                                           27

#define ANIM_TIME_US                                    2200
#define DEBOUNCE_TIME_US                                5000

// Joystick
#define UPPER_LIMIT                                     2248
#define LOWER_LIMIT                                     1848
#define FU_LIMIT                                        3096
#define FL_LIMIT                                        1000
#define JOY_MAX                                         4096

// Directions
#define MOVE_UP                                         1
#define MOVE_DOWN                                       2
#define SLOW_RIGHT                                      4
#define SLOW_LEFT                                       8
#define FAST_RIGHT                                      16
#define FAST_LEFT                                       32

#define LEFT                                            0
#define RIGHT                                           1

// Scores
#define SCORE_RECOVERED_CRYSTAL                         0 //500
#define SCORE_DROP_CRYSTAL                              1 //250
#define SCORE_GRABBER_KILL                              2 //150
#define SCORE_CHASER_KILL                               3 //250
#define SCORE_BEAMER_KILL                               4 //1000
#define SCORE_BERSERKER_KILL                            5 //150
#define SCORE_KILLER_KILL                               6 //150
#define SCORE_MINER_KILL                                7 //250

// Monster Types
#define INVADER_TYPE_COUNT                              7
#define INVADER_GRABBER                                 0
#define INVADER_KILLER                                  1
#define INVADER_CHASER                                  2
#define INVADER_BERSERKER                               3
#define INVADER_BEAMER                                  4
#define INVADER_MINER                                   5
#define INVADER_MINE                                    6
#define CRYSTAL_INDEX                                   7

#define INVADER_BASE_MOVE_SPEEED                        500000
#define INVADER_FAST_MOVE_SPEEED                        250000

#define MAX_GRABBERS                                    20
#define MAX_MINES                                       40
#define MAX_MINERS                                      10
#define MAX_BEAMERS                                     6
#define MAX_BERSERKERS                                  MAX_BEAMERS * 3
#define MAX_CHASERS                                     2

// Play Field Data
#define PLAY_FIELD_WIDTH                                640
#define SCREEN_HALF_WIDTH                               64
#define GROUND_LEVEL                                    61
#define CRYSTAL_BAND_WIDTH                              PLAY_FIELD_WIDTH / 32

// Crystal Data
#define CRYSTAL_TAKEN                                   -1
#define INVADER_KILLED                                  -1
#define NO_CRYSTAL                                      -1
#define CRYSTAL_FALL_SPEED                              500000
#define MAX_CRYSTALS                                    30

// Timers
#define TIMER_INKEY_DEBOUNCE                            0
#define TIMER_FIRE_DEBOUNCE                             1
#define TIMER_BOMB_DEBOUNCE                             2
#define TIMER_SCREEN_DRAW                               3
#define TIMER_INVADER_MOVES_SLOW                        4
#define TIMER_INVADER_MOVES_QUICK                       5
#define TIMER_CRYSTAL_MOVES                             6
#define TIMER_BULLET_MOVES                              7
#define NUMBER_OF_TIMERS                                8

// Bullets
#define MAX_BULLETS                                     20
#define BULLET_SPEED                                    250000


/*
 * STRUCTURES
 */
typedef struct {
    // Invader-specific members
    bool        pad_1;
    bool        pad_2;
    uint8_t     direction;          // Beamer
    uint8_t     flap;               // Berserker
    int8_t      crystal;            // Grabber
    int8_t      dy;                 // Killer
    int16_t     last_mine_x;        // Miner

    // Generic members
    uint8_t     type;
    int8_t      y;
    int16_t     x;
} Invader;

typedef struct {
    bool        held;
    int8_t      drop_height;
    int8_t      y;
    int16_t     x;
} Crystal;

typedef struct {
    bool        is_in_space;
    uint8_t     backdrop[(PLAY_FIELD_WIDTH >> 1)];
    uint8_t     number[INVADER_TYPE_COUNT + 1];
    Invader     *grabbers[MAX_GRABBERS];
    Invader     *killers[MAX_GRABBERS];
    Invader     *miners[MAX_MINERS];
    Invader     *mines[MAX_MINES];
    Invader     *beamers[MAX_BEAMERS];
    Invader     *berserkers[MAX_BERSERKERS];
    Invader     *chasers[MAX_CHASERS];
    Crystal     *crystals[MAX_CRYSTALS];
    Invader     **invaders[INVADER_TYPE_COUNT];
} Wave;

typedef struct {
    bool        in_play;
    bool        is_firing;
    bool        zap_state;
    uint8_t     bomb_count;
    uint8_t     lives;
    uint8_t     level;
    uint16_t    next_bonus;
    uint32_t    score;
    uint32_t    high_score;
} Game;

typedef struct {
    int8_t      y;
    int8_t      carrying;
    uint8_t     direction;
    int16_t     x;
} Player;

#include "gfx.h"

/*
 * PROTOTYPES
 */
// Initialisation Functions
void        setup();
void        init_game();
void        init_planet();
void        init_wave();

// Game Loop Management Functions
void        game_loop();
uint8_t     get_direction(uint16_t x, uint16_t y);
void        update_world();

// Player Actions Functions
void        catch_crystal();
void        place_crystal();
void        fire_laser();
void        explode_the_bomb();
bool        did_hit_invader(bool smart_bomb);
bool        did_hit_with_laser(uint16_t x, uint8_t y);

// Game Outcome Functions
void        wave_cleared();
void        check_bonus();
void        death();

// Utility Functions
bool        is_screen_split();
bool        on_screen(int16_t x);
int8_t      get_coord(int16_t x);
int16_t     get_start();


/*
 *  GLOBALS
 */
// Display
uint8_t     oled_height;
uint8_t     oled_width;
uint8_t     oled_i2c_addr;
bool        oled_inverted;

// Graphics buffer
uint8_t     oled_buffer[1024];
uint8_t     i2c_tx_buffer[1025];
uint16_t    oled_buffer_size;
uint16_t    i2c_tx_buffer_size;
uint8_t     *draw_buffer;

// Timers
uint32_t    timers[8];

// Game
Player      player;
Game        game;
Wave        wave;
int16_t     bullets[MAX_BULLETS * 4];


#endif // _PLANETI_HEADER_
