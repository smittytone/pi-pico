/*
 * Phantom Slayer
 *
 * @version     1.0.2
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _PHANTOMS_MAP_HEADER_
#define _PHANTOMS_MAP_HEADER_


/*
 * PROTOTYPES
 */
uint8_t map_init(uint8_t last_map) ;
void    show_map(uint8_t y_delta, bool show_entities);
bool    set_square_contents(uint8_t x, uint8_t y, uint8_t value);
uint8_t get_square_contents(uint8_t x, uint8_t y);
uint8_t get_view_distance(int8_t x, int8_t y, uint8_t direction);


/*
 * GLOBALS
 */
// The current map data
char *current_map[20];


/*
 * CONSTANTS
 */
#define NUMBER_OF_MAPS              6


// _PHANTOMS_MAP_HEADER_
#endif
