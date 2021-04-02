/*
 * Phantom Slayer
 *
 * @version     1.0.2
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _PHANTOMS_HEADER_
#define _PHANTOMS_HEADER_


/*
 * PROTOTYPES
 */
void    move_phantoms();
void    move_one(uint8_t c, uint8_t *x, uint8_t *y, uint8_t index);
void    manage_phantoms();
void    roll_new_phantom(uint8_t index);
uint8_t get_facing_phantom(uint8_t range);
uint8_t count_facing_phantoms(uint8_t range);
uint8_t locate_phantom(uint8_t x, uint8_t y);

uint8_t get_phantom_direction(uint8_t phantom_index);

// _PHANTOMS_HEADER_
#endif
