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
    uint8_t max_squares = get_view_distance(x, y, direction);
    uint8_t squares = 0;
    uint8_t phantom_count = 0;

    switch(direction) {
        case DIRECTION_NORTH:
            // Viewer is facing north, so left = West, right = East
            // Run through the squares from current to the view limit
            for (uint8_t i = y ; i >= 0 ; --i) {
                // Draw in the walls, floor and, if necessary, the facing wall
                bool done = draw_section(x, i, DIRECTION_WEST, DIRECTION_EAST, squares, max_squares);
                if (done) break;
                squares++;
            }

            // Run from the furthest square to the closest
            // to draw in any phantoms the viewer can see
            phantom_count = count_facing_phantoms(MAX_VIEW_RANGE);
            if (phantom_count > 0) {
                phantom_count = (phantom_count << 4) | phantom_count;
                for (uint8_t i = y ; i > y - squares ; --i) {
                    draw_phantom(x, i, y - i, &phantom_count);
                }
            }

            break;

        case DIRECTION_EAST:
            for (uint8_t i = x ; i < 20 ; ++i) {
                bool done = draw_section(i, y, DIRECTION_NORTH, DIRECTION_SOUTH, squares, max_squares);
                if (done) break;
                squares++;
            }

            phantom_count = count_facing_phantoms(MAX_VIEW_RANGE);
            if (phantom_count > 0) {
                phantom_count = (phantom_count << 4) | phantom_count;
                for (uint8_t i = x ; i < x + squares ; ++i) {
                    draw_phantom(i, y, i - x, &phantom_count);
                }
            }

            break;

        case DIRECTION_SOUTH:
            for (uint8_t i = y ; i < 20 ; ++i) {
                bool done = draw_section(x, i, DIRECTION_EAST, DIRECTION_WEST, squares, max_squares);
                if (done) break;
                squares++;
            }

            phantom_count = count_facing_phantoms(MAX_VIEW_RANGE);
            if (phantom_count > 0) {
                phantom_count = (phantom_count << 4) | phantom_count;
                for (uint8_t i = y ; i < y + squares ; ++i) {
                    draw_phantom(x, i, i - y, &phantom_count);
                }
            }

            break;

        default:
            for (uint8_t i = x ; i >= 0 ; --i) {
                bool done = draw_section(i, y, DIRECTION_SOUTH, DIRECTION_NORTH, squares, max_squares);
                if (done) break;
                squares++;
            }

            phantom_count = count_facing_phantoms(MAX_VIEW_RANGE);
            if (phantom_count > 0) {
                phantom_count = (phantom_count << 4) | phantom_count;
                for (uint8_t i = x ; i > x - squares ; --i) {
                    draw_phantom(i, y, x - i, &phantom_count);
                }
            }

            break;
    }
}


bool draw_section(uint8_t x, uint8_t y, uint8_t left, uint8_t right, uint8_t squares, uint8_t max_squares) {
    // Refactor out common code from 'draw_screen()'
    // Return 'true' when we've got to the furthest rendered square,
    // 'false' otherwise

    // Is the square a teleporter?
    if (x == game.tele_x && y == game.tele_y) {
        draw_teleporter(squares);
    }

    // Draw in left and right wall segments
    // NOTE Second argument is true or false: wall section is
    //      open or closed, respectively
    draw_left_wall(squares, (get_view_distance(x, y, left) > 0));
    draw_right_wall(squares, (get_view_distance(x, y, right) > 0));

    // Have we reached the furthest square the viewer can see?
    if (squares == max_squares) {
        draw_far_wall(squares);
        return true;
    }

    // Draw a line on the floor
    draw_floor_line(squares);
    return false;
}


void draw_floor_line(uint8_t frame_index) {
    // Draw a grid line on the floor -- this is all
    // we do to create the floor (ceiling has no line)
    Rect r = rects[frame_index + 1];
    ssd1306_line(r.x - 1, r.y + r.height, r.x + r.width + 1, r.y + r.height, 1, 1);
}


void draw_teleporter(uint8_t frame_index) {
    // Draw a grey floor tile o indicate the Escape
    // teleport location -- when stepping on this,
    // the play can beam heat to their start point
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


void draw_far_wall(uint8_t squares) {
    // Draw the wall facing the viewer, or for long distances,
    // an 'infinity' view
    //if (squares > MAX_VIEW_RANGE + 1) return;
    Rect i = rects[squares >= MAX_VIEW_RANGE ? MAX_VIEW_RANGE + 1 : squares + 1];
    ssd1306_rect(i.x, i.y, i.width, i.height, 1, true);
}


void draw_phantom(uint8_t x, uint8_t y, uint8_t frame_index, uint8_t *count) {
    // If there is a phantom at (x, y)? If so, draw it
    // TODO Allow an x-axis shift so that multiple phantoms
    //      (three max) appear side by side
    if (locate_phantom(x, y) != ERROR_CONDITION) {
        Rect r = rects[frame_index];

        uint8_t dx = 0;
        if (((*count >> 4) == 3) && ((*count & 0x0F) == 2)) {
            dx = -10;
        } else if (((*count >> 4) == 3) && ((*count & 0x0F) == 1)) {
            dx = 10;
        } else if (((*count >> 4) == 2) && ((*count & 0x0F) == 2)) {
            dx = -10;
        } else if (((*count >> 4) == 2) && ((*count & 0x0F) == 1)) {
            dx = 10
        }

        // Body
        ssd1306_rect(58 + dx, r.y + 3, 12, r.height - 3, 1, false);
        ssd1306_rect(59 + dx, r.y + 4, 10, r.height - 5, 0, true);

        // Face
        ssd1306_rect(61 + dx, r.y + 5, 6, 7 - frame_index, 1, true);

        if (frame_index < 5) {
            // Left Side
            ssd1306_line(58 + dx, r.y + 11, 58 + dx, r.y + r.height - 8, 0, 1);
            ssd1306_line(57 + dx, r.y + 11, 57 + dx, r.y + r.height - 8, 1, 1);

            // Right Side
            ssd1306_line(69 + dx, r.y + 11, 69 + dx, r.y + r.height - 8, 0, 1);
            ssd1306_line(70 + dx, r.y + 11, 70 + dx, r.y + r.height - 8, 1, 1);
        }

        // Cowl top
        ssd1306_line(60 + dx, r.y + 3, 68 + dx, r.y + 2, 0, 1);
        ssd1306_line(59 + dx, r.y + 2, 69 + dx, r.y + 1, 1, 1);
        ssd1306_line(62 + dx, r.y + 2, 64 + dx, r.y + 2, 0, 1);
        ssd1306_line(61 + dx, r.y + 1, 65 + dx, r.y + 1, 1, 1);

        *count--;
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
                memcpy(&temp_buffer[1+ (y << 7) + n], &oled_buffer[y << 7], 128 - n);
            } else {
                memcpy(&temp_buffer[1 + (y << 7)], &oled_buffer[y * 128 + n], 128 - n);
                memcpy(&temp_buffer[1 + (y << 7) + 127 - n], &side_buffer[y << 7], n);
            }
        }

        // Present the animation frame (takes approx. 20.5ms)
        i2c_tx_buffer[0] = SSD1306_WRITE_TO_BUFFER;
        i2c_write_block(&i2c_tx_buffer[0], oled_buffer_size + 1);
    }
}