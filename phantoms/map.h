#ifndef _PHANTOMS_MAP_HEADER_
#define _PHANTOMS_MAP_HEADER_


/*
 * PROTOTYPES
 */
void map_init();
void show_map(uint8_t y_delta);

uint8_t get_view_distance(uint8_t x, uint8_t y, uint8_t direction);
uint8_t get_square_contents(uint8_t x, uint8_t y);


/*
 * GLOBALS
 */
// The current map data
char *current_map[20];


// _PHANTOMS_MAP_HEADER_
#endif