/*
 * Phantoms
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#include "main.h"


/*
 *  Wall perspective triangles
 */
const char angles[2][13] = {
    "\x3F\x3E\x3E\x3E\x3C\x3C\x3C\x38\x38\x38\x30\x30\x30",
    "\xFE\x7E\x7E\x7E\x3E\x3E\x3E\x1E\x1E\x1E\x1C\x1C\x1C"
};


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
    //uint8_t squares = 0;

    switch(direction) {
        case DIRECTION_NORTH:
            // Viewer is facing north, so left = West, right = East
            // Run through the squares from current to the view limit
            i = y - last_frame;
            do {
                draw_section(x, i, DIRECTION_WEST, DIRECTION_EAST, frame, last_frame);
                if (phantom_count > 0 && locate_phantom(x, i) != ERROR_CONDITION) {
                    draw_phantom(x, i, frame, &phantom_count);
                }
                --frame;
                ++i;
            } while (frame >= 0);
        break;

        case DIRECTION_EAST:
            i = x + last_frame;
            do {
                draw_section(i, y, DIRECTION_NORTH, DIRECTION_SOUTH, frame, last_frame);
                if (phantom_count > 0 && locate_phantom(i, y) != ERROR_CONDITION) {
                    draw_phantom(i, y, frame, &phantom_count);
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
                    draw_phantom(x, i, frame, &phantom_count);
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
                    draw_phantom(i, y, frame, &phantom_count);
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

    // Is the square a teleporter?
    if (x == game.tele_x && y == game.tele_y) {
        draw_teleporter(current_frame);
    }

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
    // Draw a grey floor tile o indicate the Escape teleport location.
    // When stepping on this, the player can beam heat to their start point
    Rect r = rects[frame_index];
    bool dot_state = true;

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


void draw_phantom(uint8_t x, uint8_t y, uint8_t frame_index, uint8_t *count) {
    // If there is a phantom at (x, y)? If so, draw it
    // TODO Allow an x-axis shift so that multiple phantoms
    //      (three max) appear side by side
    Rect r = rects[frame_index];

    uint8_t dx = 64;
    uint8_t c = *count;
    uint8_t number_phantoms = c >> 4;
    uint8_t current = c & 0x0F;

    if (number_phantoms > 1) {
        if (current == 2) dx = 64 - r.spot;
        if (current == 1) dx = 64 + r.spot;
        *count = c - 1;
    }

    uint8_t p_height = r.height - 4;
    uint8_t f_width = 6 - frame_index;
    if ((f_width & 0x01) > 0) f_width += 1;
    if (f_width == 0) f_width = 2;
    uint8_t p_width = f_width << 1;
    uint8_t bx = dx - (p_width >> 1);

    // Body outer frame
    ssd1306_rect(bx, r.y + 2, p_width, p_height, 1, false);
    // Body inner fill
    ssd1306_rect(bx + 1, r.y + 3, (p_width - 2), p_height - 2, 0, true);

    // Face fill
    ssd1306_rect(dx - (f_width >> 1), r.y + 5, f_width, 7 - frame_index, 1, true);

    if (frame_index < 5) {
        // Left arm
        ssd1306_line(bx,     r.y + 12 - frame_index, bx,     32, 0, 1);
        ssd1306_line(bx - 1, r.y + 12 - frame_index, bx - 1, 32, 1, 1);

        // Right arm
        ssd1306_line(bx + p_width - 1, r.y + 12 - frame_index, bx + p_width - 1, 32, 0, 1);
        ssd1306_line(bx + p_width,     r.y + 12 - frame_index, bx + p_width,     32, 1, 1);
    }

    // Cowl top
    p_width -= 1;
    ssd1306_line(bx + 1, r.y + 1, bx + p_width, r.y + 2, 1, 1);
    ssd1306_line(bx + 1, r.y + 2, bx + p_width, r.y + 2, 0, 1);
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