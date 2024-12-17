/*
 * Phantom Slayer
 *
 * @version     1.0.3
 * @author      smittytone
 * @copyright   2024, Tony Smith
 * @licence     MIT
 *
 */
#include "main.h"

extern uint8_t oled_height;
extern uint8_t oled_width;
extern uint8_t oled_i2c_addr;
extern bool oled_inverted;
extern Rect rects[7];
extern Game game;
extern uint8_t oled_buffer[1024];
extern uint8_t temp_buffer[1024];
extern uint8_t side_buffer[1024];
extern uint8_t i2c_tx_buffer[1025];
extern uint16_t oled_buffer_size;
extern uint16_t i2c_tx_buffer_size;
extern uint8_t *draw_buffer;
extern uint8_t player_x;
extern uint8_t player_y;
extern uint8_t player_direction;


/*
 *  Wall perspective triangles
 */
const char angles[2][13] = {
    "\x3F\x3E\x3E\x3E\x3C\x3C\x3C\x38\x38\x38\x30\x30\x30",
    "\xFE\x7E\x7E\x7E\x3E\x3E\x3E\x1E\x1E\x1E\x1C\x1C\x1C"
};

/*
 *  Phantom shape graphics
 *  There are shapes for the mask + border, and the Phantom itself,
 *  each at set distance from the player defined by the frame rects
 */

const char phantom_frnt_00[144] =
{"\x00\x00\x00\x00\x00\x00\x07\x1C\x38\x70\xF0\x78\x1D\x07\x00\x00\x00\x00\x03\x07\x07\x0F\x0F\x1F\xDF\xFF\x7F\x0F\x3F\x7F\xFF\x9F\x0F\x0F\x07\x07\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xF0\xF0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xF0\xF0\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\x00\x00"};

const char phantom_back_00[160] =
{"\x00\x00\x00\x00\x00\x00\x07\x1F\x3F\x7F\xFF\xFF\xFF\x7F\x1F\x0F\x00\x00\x00\x00\x03\x07\x07\x0F\x0F\x1F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x0F\x0F\x07\x07\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xFC\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xFC\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\xFC\x00\x00"};

const char phantom_frnt_01[105] =
{"\x00\x00\x00\x00\x00\x0F\x19\x60\xE0\x70\x1B\x0F\x00\x00\x00\x0F\x1F\x3F\x3F\x7F\x7F\xFF\xFF\xFF\xFF\xFF\x7F\x3F\x1F\x1F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xFC\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xFC\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\x00\x00"};

const char phantom_back_01[119] =
{"\x00\x00\x00\x00\x00\x0F\x1F\x7F\xFF\xFF\xFF\x7F\x1F\x0F\x00\x00\x00\x0F\x1F\x3F\x3F\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x3F\x1F\x1F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\xF0\x00\x00"};

const char phantom_frnt_02[60] =
{"\x00\x00\x00\x01\x1D\x73\xC3\x63\x37\x1D\x00\x00\x3F\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00"};

const char phantom_back_02[84] =
{"\x00\x00\x00\x01\x1F\x7F\xFF\xFF\xFF\x7F\x3F\x1F\x00\x00\x3F\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0\xC0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0\xC0\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\xC0\x00\x00"};

const char phantom_frnt_03[36] =
{"\x00\x03\x07\x3F\xC7\x47\x3F\x01\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC0\xFF\xFF\xFF\xFF\xFF\xFF\xC0\xC0\x00\xFC\xFC\xFC\xFC\xFC\xFC\x00\x00"};

const char phantom_back_03[55] =
{"\x00\x01\x03\x1F\x7F\xFF\x7F\x3F\x1F\x00\x00\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xFC\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x80\x80\x80\x80\x80\x80\x80\x80\x00\x00"};

const char phantom_frnt_04[18] =
{"\x07\x1F\x7F\x9F\x7F\x07\xF0\xFF\xFF\xFF\xFF\xF0\x00\xF0\xF0\xF0\xF0\x00"};

const char phantom_back_04[24] =
{"\x03\x0F\x3F\x7F\xFF\x7F\x3F\x03\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFE\x00\xFC\xFC\xFC\xFC\xFC\xFC\x00"};

const char phantom_frnt_05[6] =
{"\x3C\xFF\x3C\x00\xC0\x00"};

const char phantom_back_05[10] =
{"\x3F\xFF\xFF\xFF\x3F\x00\xF0\xF0\xF0\x00"};


/*
 * GRAPHICS FUNCTIONS
 */
void draw_screen(uint8_t x, uint8_t y, uint8_t direction) {
    // Render a single viewpoint frame at the specified square.
    // Progressively draw in walls, square by square, moving away
    // from (x,y) in the specified direction
    int8_t last_frame = get_view_distance(x, y, direction);
    int8_t frame = last_frame;
    uint8_t phantom_count = count_facing_phantoms(last_frame);
    phantom_count = (phantom_count << 4) | phantom_count;
    uint8_t i = 0;

    switch(direction) {
        case DIRECTION_NORTH:
            // Viewer is facing north, so left = West, right = East
            // Run through the squares from the view limit (inner frame) forward
            // to the player's current square (outer frame)
            i = y - last_frame;
            do {
                // Draw the current frame
                draw_section(x, i, DIRECTION_WEST, DIRECTION_EAST, frame, last_frame);

                // Check for the presence of a Phantom on the drawn square
                // and, if there is, draw it in
                // NOTE 'phantom_count comes back so we can keep track of multiple
                //      Phantoms in the player's field of view and space them
                //      laterally
                if (phantom_count > 0 && locate_phantom(x, i) != ERROR_CONDITION) {
                    draw_phantom(frame, &phantom_count);
                }

                // Move to the next frame and square
                --frame;
                ++i;
            } while (frame >= 0);
        break;

        case DIRECTION_EAST:
            i = x + last_frame;
            do {
                draw_section(i, y, DIRECTION_NORTH, DIRECTION_SOUTH, frame, last_frame);
                if (phantom_count > 0 && locate_phantom(i, y) != ERROR_CONDITION) {
                    draw_phantom(frame, &phantom_count);
                }
                --frame;
                --i;
            } while (frame >= 0);
            break;

        case DIRECTION_SOUTH:
            i = y + last_frame;
            do {
                draw_section(x, i, DIRECTION_EAST, DIRECTION_WEST, frame, last_frame);
                if (phantom_count > 0 && locate_phantom(x, i) != ERROR_CONDITION) {
                    draw_phantom(frame, &phantom_count);
                }
                --frame;
                --i;
            } while (frame >= 0);
            break;

        default:
            i = x - last_frame;
            do {
                draw_section(i, y, DIRECTION_SOUTH, DIRECTION_NORTH, frame, last_frame);
                if (phantom_count > 0 && locate_phantom(i, y) != ERROR_CONDITION) {
                    draw_phantom(frame, &phantom_count);
                }
                --frame;
                ++i;
            } while (frame >= 0);
    }
}


bool draw_section(uint8_t x, uint8_t y, uint8_t left_dir, uint8_t right_dir, uint8_t current_frame, uint8_t furthest_frame) {
    // Refactor out common code from 'draw_screen()'
    // Return 'true' when we've got to the furthest rendered square,
    // 'false' otherwise

    // Is the square a teleporter? If so, draw it
    if (x == game.tele_x && y == game.tele_y) draw_teleporter(current_frame);

    // Draw in left and right wall segments
    // NOTE Second argument is true or false: wall section is
    //      open or closed, respectively
    draw_left_wall(current_frame, (get_view_distance(x, y, left_dir) > 0));
    draw_right_wall(current_frame, (get_view_distance(x, y, right_dir) > 0));

    // Have we reached the furthest square the viewer can see?
    if (current_frame == furthest_frame) {
        draw_far_wall(current_frame);
        return true;
    }

    // Draw a line on the floor
    draw_floor_line(current_frame);
    return false;
}


void draw_floor_line(uint8_t frame_index) {
    // Draw a grid line on the floor -- this is all
    // we do to create the floor (ceiling has no line)
    Rect r = rects[frame_index + 1];
    ssd1306_line(r.x - 1, r.y + r.height, r.x + r.width + 1, r.y + r.height, 1, 1);
}


void draw_teleporter(uint8_t frame_index) {
    // Draw a grey floor tile to indicate the Escape teleport location.
    // When stepping on this, the player can beam to their start point
    Rect r = rects[frame_index];
    bool dot_state = true;

    // Plot a dot pattern
    for (uint8_t y = r.y + r.height -  4; y < r.y + r.height ; ++y) {
        for (uint8_t i = r.x ; i < r.x + r.width - 2; i += 2) {
            ssd1306_plot(dot_state ? i : i + 1, y, 1);
        }
        dot_state = !dot_state;
    }
}


void draw_left_wall(uint8_t frame_index, bool is_open) {
    // Render a left-side wall section for the current square
    // NOTE 'is_open' is true if there is no wall -- we're at
    //      a junction point

    // 'i'ner and 'o'uter frames
    Rect i = rects[frame_index + 1];
    Rect o = rects[frame_index];

    // Draw an open left wall, ie. the facing wall of the
    // adjoining corridor, and then return
    ssd1306_rect(o.x, i.y, i.x - o.x - 1, i.height, 1, true);
    if (is_open) return;

    // Add upper and lower triangles to present a wall section
    uint8_t byte = 0;
    for (uint8_t k = 0 ; k < i.x - 1 - o.x ; ++k) {
        // Upper triangle
        byte =  angles[0][k];
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if ((byte & (1 << j)) != 0) {
                ssd1306_plot(o.x + k, o.y + j, 1);
            }
        }

        // Lower triangle
        byte = angles[1][k];
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if ((byte & (1 << j)) != 0) {
                ssd1306_plot(o.x + k, o.y + o.height - 9 + j, 1);
            }
        }
    }
}


void draw_right_wall(uint8_t frame_index, bool is_open) {
    // Render a right-side wall section for the current square
    // NOTE 'is_open' is true if there is no wall -- we're at
    //      a junction point

    // 'i'ner and 'o'uter frames
    Rect i = rects[frame_index + 1];
    Rect o = rects[frame_index];

    // Draw an open left wall, ie. the facing wall of the
    // adjoining corridor, and then return
    uint8_t xd = i.width + i.x;
    ssd1306_rect(xd + 1, i.y, (o.width + o.x) - xd - 1, i.height, 1, true);
    if (is_open) return;

    // Add upper and lower triangles to present a wall section
    uint8_t byte = 0;
    uint8_t max = (o.width + o.x) - xd - 1;
    for (uint8_t k = 0 ; k < max ; ++k) {
        // Upper triangle
        byte = angles[0][k];
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if ((byte & (1 << j)) > 0) {
                ssd1306_plot(o.width + o.x - k - 1, o.y + j, 1);
            }
        }

        // Lower triangle
        byte = angles[1][k];
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if ((byte & (1 << j)) > 0) {
                ssd1306_plot(o.width + o.x - k - 1, o.y + o.height - 9 + j, 1);
            }
        }
    }
}


void draw_far_wall(uint8_t frame_index) {
    // Draw the wall facing the viewer, or for very long distances,
    // an 'infinity' view
    Rect i = rects[frame_index + 1];
    ssd1306_rect(i.x, i.y, i.width, i.height, 1, true);
}


void draw_phantom(uint8_t frame_index, uint8_t *count) {
    // Draw a Phantom in the specified frame - which determines
    // its x and y co-ordinates in the frame
    Rect r = rects[frame_index];
    uint8_t dx = 64;
    uint8_t c = *count;
    uint8_t number_phantoms = c >> 4;
    uint8_t current = c & 0x0F;
    uint8_t width = 9;

    // Space the phantoms sideways ccording to
    // the number of them on screen
    if (number_phantoms > 1) {
        if (current == 2) dx = 64 - r.spot;
        if (current == 1) dx = 64 + r.spot;
        *count = c - 1;
    }

    // NOTE Screen render frame indices run from 0 to 5, front to back
    switch(frame_index) {
        case 0:
            width = 18;
            dx -= (width >> 1);
            draw_bitmap(dx - 1, r.y + 1, 20,    1, 160, &phantom_back_00[0]);
            draw_bitmap(dx,     r.y + 2, width, 0, 144, &phantom_frnt_00[0]);
            break;
        case 1:
            width = 15;
            dx -= (width >> 1);
            draw_bitmap(dx - 1, r.y + 1, 17,    1, 119, &phantom_back_01[0]);
            draw_bitmap(dx,     r.y + 2, width, 0, 105, &phantom_frnt_01[0]);
            break;
        case 2:
            width = 12;
            dx -= (width >> 1);
            draw_bitmap(dx - 1, r.y + 1, 14,    1, 84, &phantom_back_02[0]);
            draw_bitmap(dx,     r.y + 2, width, 0, 60, &phantom_frnt_02[0]);
            break;
        case 3:
            width = 9;
            dx -= (width >> 1);
            draw_bitmap(dx - 1, r.y    , 11,    1, 55, &phantom_back_03[0]);
            draw_bitmap(dx,     r.y + 2, width, 0, 36, &phantom_frnt_03[0]);
            break;
        case 4:
            width = 6;
            dx -= (width >> 1);
            draw_bitmap(dx - 1, r.y + 1, 8,     1, 24, &phantom_back_04[0]);
            draw_bitmap(dx,     r.y + 2, width, 0, 18, &phantom_frnt_04[0]);
            break;
        default:
            width = 3;
            dx -= (width >> 1);
            draw_bitmap(dx - 1, r.y + 1, 5,     0, 10, &phantom_frnt_05[0]);
            draw_bitmap(dx,     r.y + 2, width, 1, 6,  &phantom_frnt_05[0]);
    }
}


void draw_bitmap(uint8_t x, uint8_t y, uint8_t width, uint8_t colour, uint8_t length, const char *bitmap) {
    // Paint the specifiec monochrome bitmap to the screen
    // with (x,y) the top-left co-ordinate. Zeros in the bit map are the
    // 'alpha channel', Ones are set or unset according to the value of
    // 'colour'. The value of 'length' is the number of bytes in the bitmap;
    // 'width' is the number of bytes per row in the image. Bytes are vertical,
    // with bit 0 at the top.

    // Save the left-hand co-ord
    uint8_t x_start = x;

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


void animate_turn(bool is_left) {
    // Animate slide (left or right) from the current view
    // to a pre-rendered side-view buffer

    // Draw the side view - the view the player will see next -
    // to the side buffer
    draw_buffer = &side_buffer[0];
    ssd1306_clear();
    draw_screen(player_x, player_y, player_direction);

    // Set the drawing buffer back to the main buffer
    draw_buffer = &oled_buffer[0];

    // Slide across from the current view (in 'oled_buffer')
    // to the next view (in 'side_buffer') by copying the
    // key segments to the temporary buffer (the current drawing buffer)
    // NOTE We write directly to 'i2c_tx_buffer' to save a 1KB memcpy()
    for (uint8_t n = 1 ; n < 128 ; n += 16) {
        // Draw row by row
        for (uint8_t y = 0 ; y < 8 ; ++y) {
            // Write out a line of left-buffer bytes followed by
            // some right-buffer bytes
            if (is_left) {
                memcpy(&i2c_tx_buffer[1 + (y << 7)], &side_buffer[(y << 7) + 128 - n], n);
                memcpy(&i2c_tx_buffer[1+ (y << 7) + n], &oled_buffer[y << 7], 128 - n);
            } else {
                memcpy(&i2c_tx_buffer[1 + (y << 7)], &oled_buffer[y * 128 + n], 128 - n);
                memcpy(&i2c_tx_buffer[1 + (y << 7) + 127 - n], &side_buffer[y << 7], n);
            }
        }

        // Present the animation frame (takes approx. 20.5ms)
        i2c_tx_buffer[0] = SSD1306_WRITE_TO_BUFFER;
        i2c_write_block(&i2c_tx_buffer[0], oled_buffer_size + 1);
    }
}
