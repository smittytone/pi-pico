/*
 * Phantoms
 *
 * @version     1.0.1
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
void    draw_screen(uint8_t x, uint8_t y, uint8_t direction);
bool    draw_section(uint8_t x, uint8_t y, uint8_t left, uint8_t right, uint8_t squares, uint8_t max_squares);
void    draw_floor_line(uint8_t inset);
void    draw_teleporter(uint8_t inset);
void    draw_left_wall(uint8_t steps, bool left_open);
void    draw_right_wall(uint8_t steps, bool right_open);
void    draw_far_wall(uint8_t steps);
void    draw_phantom(uint8_t frame_index, uint8_t *count);
void    draw_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t c, uint8_t len, const char *bitmap);
void    bitmap_plot(int8_t x, int8_t y, uint8_t char_bit, uint16_t char_byte, uint8_t byte_bit, uint8_t colour);
void    animate_turn(bool is_left);


// _PHANTOMS_GFX_HEADER_
#endif
