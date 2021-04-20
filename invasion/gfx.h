/*
 * Planet Invasion for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _PLANET_INVASION_GFX_HEADER_
#define _PLANET_INVASION_GFX_HEADER_


/*
 * PROTOTYPES
 */
void    draw_ship(int8_t x, int8_t y, uint8_t direction, bool is_firing, bool is_carrying);
void    draw_bomb(int8_t x, int8_t y);
void    draw_boom(int8_t x, int8_t y);
void    draw_monster(Invader *i);
void    draw_bitmap(int8_t x, int8_t y, uint8_t width, uint8_t colour, uint8_t length, const char *bitmap);
void    bitmap_plot(int8_t x, int8_t y, uint8_t char_bit, uint16_t char_byte, uint8_t byte_bit, uint8_t colour);


#endif  // _PLANET_INVASION_GFX_HEADER_
