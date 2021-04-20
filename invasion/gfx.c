/*
 * Planet Invasion for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#include "invasion.h"



/*
 *  Phantom shape graphics
 *  There are shapes for the mask + border, and the Phantom itself,
 *  each at set distance from the player defined by the frame rects
 */
const char ship_right[7]  = "\xE0\xE0\x60\x60\x60\x20\x20";
const char ship_left[7]   = "\x20\x20\x60\x60\x60\xE0\xE0";
const char smart_bomb[8]  = "\xA0\x40\xE0\xE0\xE0\xE0\xE0\x40";
const char boom[8]        = "\x45\xAA\x55\x0A\x51\xAA\x44\xAA";

const char grabber[8]     = "\x76\x56\x58\xF8\xF8\x58\x56\x76";
const char killer[8]      = "\xDC\x54\x34\x7E\x7E\x34\x54\xDC";

const char chaser[8]      = "\x60\x90\x90\xF0\xF0\x90\x90\x60";

const char beamer_01[8]   = "\x28\x28\x28\x38\x7C\x6C\x6E\xEE";
const char beamer_02[8]   = "\xEE\x6E\x6C\x7C\x38\x28\x28\x28";
const char berserker_01[] = "\x80\x40\x28\x16\x16\x28\x40\x80";
const char berserker_02[] = "\x40\x40\x28\x16\x16\x28\x40\x40";
const char berserker_03[] = "\x10\x20\x28\x16\x16\x28\x20\x10";

const char miner[4]       = "\xEE\xEE\xBA\xBA";
const char mine[6]        = "\x84\x48\x30\x30\x48\x84";



void draw_ship(int8_t x, int8_t y, uint8_t direction, bool is_firing, bool is_carrying) {
    const char *ship = (direction == 0 ? &ship_left[0] : &ship_right[0]);
    draw_bitmap(x, y, 7, 1, 7, ship);

    if (is_firing) {
        // Assume player is facing left
        uint8_t x1 = x - 2 - 20;
        uint8_t x2 = x - 2;

        if (direction == RIGHT) {
            x1 = x + 8 + 20;
            x2 = x + 8;
        }

        // Draw in the zap
        ssd1306_line(x1, y + 2, x2, y + 2, 1, 1);

        // Has the laser zapped any bullets? Just clear any that it has
        for (uint8_t i = 0 ; i < MAX_BULLETS * 4 ; i += 4) {
            if ((bullets[i] == NO_CRYSTAL) || (bullets[i + 1] != y + 2)) continue;
            if ((bullets[i] >= x1) && (bullets[i] <= x2)) bullets[i] = NO_CRYSTAL;
        }
    }

    if (is_carrying) {
        if (direction == RIGHT) x += 6;
        ssd1306_line(x,     y + 3, x,     y + 6, 1, 1);
        ssd1306_line(x + 1, y + 3, x + 1, y + 6, 1, 1);
    }
}


void draw_bomb(int8_t x, int8_t y) {
    // Draw a smartbomb icon
    draw_bitmap(x, y, 8, 1, 8, smart_bomb);
}


void draw_boom(int8_t x, int8_t y) {
    // Draw an explosion icon
    draw_bitmap(x, y, 8, 1, 8, boom);
}


void draw_monster(Invader *i) {
    // Draw the monster -- should have already
    // checked that it's on screen
    if (i == NULL) return;

    const char *b;
    uint8_t l = 8;
    bool is_carrying = false;

    switch(i->type) {
        case INVADER_CHASER:
            b = &chaser[0];
            break;
        case INVADER_KILLER:
            b = &killer[0];
            break;
        case INVADER_BEAMER:
            b = i->direction == LEFT ? &beamer_01[0] : &beamer_02[0];
            break;
        case INVADER_BERSERKER:
            switch(i->flap) {
                case 0:
                    b = &berserker_01[0];
                    break;
                case 1:
                    b = &berserker_02[0];
                    break;
                default:
                    b = &berserker_03[0];
            }

            i->flap = (i->flap == 2) ? 0 : (i->flap + 1);
            break;
        case INVADER_MINER:
            l = 4;
            b = &miner[0];
            break;
        case INVADER_MINE:
            l = 6;
            b = &mine[0];
            break;
        default:
            b = &grabber[0];
            is_carrying = (i->crystal != NO_CRYSTAL);
    }

    uint8_t x = get_coord(i->x);
    draw_bitmap(x, i->y, l, 1, l, b);

    if (is_carrying) {
        if ((x + 6 >= 0) && (x + 6 < 128)) ssd1306_line(x + 6, i->y + 8, x + 6, i->y + 10, 1, false);
        if ((x + 7 >= 0) && (x + 7 < 128)) ssd1306_line(x + 7, i->y + 8, x + 7, i->y + 10, 1, false);
    }
}


void draw_bitmap(int8_t x, int8_t y, uint8_t width, uint8_t colour, uint8_t length, const char *bitmap) {
    // Paint the specifiec monochrome bitmap to the screen
    // with (x,y) the top-left co-ordinate. Zeros in the bit map are the
    // 'alpha channel', Ones are set or unset according to the value of
    // 'colour'. The value of 'length' is the number of bytes in the bitmap;
    // 'width' is the number of bytes per row in the image. Bytes are vertical,
    // with bit 0 at the top.

    // Save the left-hand co-ord
    int8_t x_start = x;

    for (uint8_t i = 0 ; i < length ; ++i) {
        // Get the column byte
        uint8_t col = ssd1306_text_flip(bitmap[i]);

        // Get the topmost bit (in range 0-7)
        uint8_t z = (y - ((y >> 3) << 3)) - 1;

        // Run through the bits in the column, setting the destintation
        // bit accordingly
        for (uint8_t k = 0 ; k < 8 ; ++k) {
            if (((y + k) % 8) == 0 && k > 0) {
                z = 0;
            } else {
                z += 1;
            }

            if (x < oled_width) bitmap_plot(x, y, k, col, z, colour);
        }

        // Move onto the next byte along, cycling back when we
        // get to the end of the width of the image
        x++;
        if (x >= x_start + width) {
            x = x_start;
            y += 8;
        }
    }
}


void bitmap_plot(int8_t x, int8_t y, uint8_t char_bit, uint16_t char_byte, uint8_t byte_bit, uint8_t colour) {
    // Write a byte from bitmap to the screen buffer
    if (x < 0 || x >= oled_width) return;
    if (y + char_bit < 0 || y + char_bit >= oled_height) return;
    uint16_t byte = ssd1306_coords_to_index(x, y + char_bit);

    if (colour == 1) {
        // Set the buffer pixel if it's set in the char byte
        if ((char_byte & (1 << char_bit)) != 0) draw_buffer[byte] |= (1 << byte_bit);
    } else {
        // Clear the buffer pixel if it's set in the char byte
        if ((char_byte & (1 << char_bit)) != 0) draw_buffer[byte] &= ~(1 << byte_bit);
    }
}
