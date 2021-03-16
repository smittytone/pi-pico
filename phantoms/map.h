/*
 * Phantoms
 *
 * @version     1.0.0
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
void map_init();
void show_map(uint8_t y_delta, bool show_entities);

uint8_t get_view_distance(uint8_t x, uint8_t y, uint8_t direction);
uint8_t get_square_contents(uint8_t x, uint8_t y);
bool set_square_contents(uint8_t x, uint8_t y, uint8_t value);


/*
 * GLOBALS
 */
// The current map data
char *current_map[20];


// _PHANTOMS_MAP_HEADER_
#endif