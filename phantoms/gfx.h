/*
 * Phantoms
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _PHANTOMS_GFX_HEADER_
#define _PHANTOMS_GFX_HEADER_


/*
 * PROTOTYPES
 */
void draw_screen(uint8_t x, uint8_t y, uint8_t direction);
bool draw_section(uint8_t x, uint8_t y, uint8_t left, uint8_t right, uint8_t squares, uint8_t max_squares);
void draw_floor_line(uint8_t inset);
void draw_teleporter(uint8_t inset);
void draw_left_wall(uint8_t steps, bool left_open);
void draw_right_wall(uint8_t steps, bool right_open);
void draw_far_wall(uint8_t steps);
void draw_dir_arrow();
void draw_phantom(uint8_t x, uint8_t y, uint8_t c);
void move_phantoms();


// _PHANTOMS_GFX_HEADER_
#endif