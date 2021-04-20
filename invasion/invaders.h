/*
 * Planet Invasion for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _PLANET_INVASION_INVADER_HEADER_
#define _PLANET_INVASION_INVADER_HEADER_


/*
 * PROTOTYPES
 */
void        move_monsters();
uint8_t     live_monster_count();
uint8_t     count_invader(uint8_t kind);
uint8_t     get_max_invaders(uint8_t kind);
void        free_invaders(uint8_t kind);
int8_t      new_invader(uint8_t kind);
int8_t      check_y(int8_t y);
int16_t     check_x(int16_t x);

void        init_grabbers(uint8_t number);
void        move_grabbers();

void        init_miners();
void        move_miners();

void        init_beamers();
void        move_beamers();

void        init_killers();
void        new_killer(int16_t x);
void        move_killers();

void        init_mines();
void        new_mine(int16_t x, int8_t y);

void        init_berserkers();
void        new_berserkers(int16_t x, int8_t y);
void        move_berserkers();

void        init_chasers();
void        new_chaser();




#endif  // _PLANET_INVASION_INVADER_HEADER_
