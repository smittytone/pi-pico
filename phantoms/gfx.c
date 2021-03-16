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
    // Render a single frame:
    // Progressively draw in walls, square by square, in the
    // direction of the player’s viewpoint, in to out
    uint8_t max_squares = get_view_distance(x, y, direction);
    uint8_t squares = 0;

    switch(direction) {
        case DIRECTION_NORTH:
            // Facing north, so left = West, right = Eest
            // Run through squares from current to map limit
            for (uint8_t i = y ; i >= 0 ; --i) {
                bool done = draw_section(x, i, DIRECTION_WEST, DIRECTION_EAST, squares, max_squares);
                if (!done) squares++;
            }

            for (uint8_t i = y ; i > y - squares ; --i) {
                draw_phantom(x, i, y - i);
            }

            break;

        case DIRECTION_EAST:
            for (uint8_t i = x ; i < 20 ; ++i) {
                bool done = draw_section(i, y, DIRECTION_NORTH, DIRECTION_SOUTH, squares, max_squares);
                if (!done) squares++;
            }

            for (uint8_t i = x ; i < x + squares ; ++i) {
                draw_phantom(i, y, i - x);
            }

            break;

        case DIRECTION_SOUTH:
            for (uint8_t i = y ; i < 20 ; ++i) {
                bool done = draw_section(x, i, DIRECTION_EAST, DIRECTION_WEST, squares, max_squares);
                if (!done) squares++;
            }

            for (uint8_t i = y ; i < y + squares ; ++i) {
                draw_phantom(x, i, i - y);
            }

            break;

        default:
            for (uint8_t i = x ; i >= 0 ; --i) {
                bool done = draw_section(i, y, DIRECTION_SOUTH, DIRECTION_NORTH, squares, max_squares);
                if (!done) squares++;
            }

            for (uint8_t i = x ; i > x - squares ; --i) {
                draw_phantom(i, y, x - i);
            }

            break;
    }
}


bool draw_section(uint8_t x, uint8_t y, uint8_t left, uint8_t right, uint8_t squares, uint8_t max_squares) {
    // Refactor out common code from 'draw_screen()'
    // Return 'true' when we've got to the end wall, 'false' otherwise
    if (get_square_contents(x, y) == MAP_TILE_TELEPORTER) {
        draw_teleporter(squares);
    }

    draw_left_wall(squares, (get_view_distance(x, y, left) > 0));
    draw_right_wall(squares, (get_view_distance(x, y, right) > 0));

    if (squares == max_squares) {
        draw_far_wall(squares);
        return true;
    }

    draw_floor_line(squares);
    return false;
}


void draw_floor_line(uint8_t square) {
    // Draw a grid line on the floor -- this is all
    // we do to create the floor
    Rect r = rects[square + 1];
    ssd1306_line(r.origin_x - 1, r.origin_y + r.height, r.origin_x + r.width + 1, r.origin_y + r.height, 1, 1);
}


void draw_teleporter(uint8_t frame_index) {
    // Draw a grey floor to indicate the Escape
    // teleport location -- after stepping on this,
    // the play can beam heat at any time, but only once
    // per level
    Rect r = rects[frame_index + 1];
    bool dot_state = true;

    for (uint8_t y = r.origin_y + r.height -  4; y < r.origin_y + r.height ; ++y) {
        for (uint8_t i = r.origin_x ; i < r.origin_x + r.width - 2; i += 2) {
            ssd1306_plot(dot_state ? i : i + 1, y, 1);
        }
        dot_state = !dot_state;
    }
}


void draw_left_wall(uint8_t frame_index, bool is_open) {
    // Draw main left-side rect
    Rect i = rects[frame_index + 1];
    Rect o = rects[frame_index];

    // Draw an open left wall
    ssd1306_rect(o.origin_x, i.origin_y, i.origin_x - o.origin_x - 1, i.height, 1, true);
    if (is_open) return;

    // Add upper and lower triangles for a closed wall
    uint8_t byte = 0;
    for (uint8_t k = 0 ; k < i.origin_x - 1 - o.origin_x ; ++k) {
        // Upper triangle
        byte =  angles[0][k];
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if ((byte & (1 << j)) != 0) {
                ssd1306_plot(o.origin_x + k, o.origin_y + j, 1);
            }
        }

        // Lower triangle
        byte = angles[1][k];
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if ((byte & (1 << j)) != 0) {
                ssd1306_plot(o.origin_x + k, o.origin_y + o.height - 9 + j, 1);
            }
        }
    }
}


void draw_right_wall(uint8_t frame_index, bool is_open) {
    // Draw main right-side rect
    Rect i = rects[frame_index + 1];
    Rect o = rects[frame_index];
    uint8_t xd = i.width + i.origin_x;
    ssd1306_rect(xd + 1, i.origin_y, (o.width + o.origin_x) - xd - 1, i.height, 1, true);
    if (is_open) return;

    // Add upper and lower triangles for a closed wall
    uint8_t byte = 0;
    uint8_t max = (o.width + o.origin_x) - xd - 1;
    for (uint8_t k = 0 ; k < max ; ++k) {
        // Upper triangle
        byte = angles[0][k];
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if ((byte & (1 << j)) > 0) {
                ssd1306_plot(o.width + o.origin_x - k - 1, o.origin_y + j, 1);
            }
        }

        // Lower triangle
        byte = angles[1][k];
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if ((byte & (1 << j)) > 0) {
                ssd1306_plot(o.width + o.origin_x - k - 1, o.origin_y + o.height - 9 + j, 1);
            }
        }
    }
}


void draw_far_wall(uint8_t squares) {
    // Draw the wall facing the player, or for long distances,
    // an 'infinity' view
    if (squares > MAX_VIEW_RANGE) return;
    Rect i = rects[squares > MAX_VIEW_RANGE - 2 ? MAX_VIEW_RANGE : squares + 1];
    ssd1306_rect(i.origin_x, i.origin_y, i.width, i.height, 1, true);
}


void draw_phantom(uint8_t x, uint8_t y, uint8_t c) {
    // Run through the list of phantoms to see if
    // the player is facing any of them
    // 'x' and 'y' are iterated co-ordinates from the most
    // distant square up to the player
    if (locate_phantom(x, y) != ERROR_CONDITION) {
        Rect r = rects[c];
        ssd1306_rect(58, r.origin_y + 2, 12, r.height - 3, 1, false);
        ssd1306_rect(59, r.origin_y + 3, 10, r.height - 5, 0, true);
        ssd1306_rect(61, r.origin_y + 5, 6, 6, 1, true);
    }
}


void draw_dir_arrow() {
    // Debug route - draw a direction indication arrow
    if (player_direction == 0 || player_direction == 2) {
        ssd1306_line(64,0,64,4,1,1);
        if (player_direction == 0) {
            ssd1306_plot(63,1,1);
            ssd1306_plot(65,1,1);
        } else {
            ssd1306_plot(63,3,1);
            ssd1306_plot(65,3,1);
        }
    } else {
        ssd1306_line(62,2,66,2,1,1);
        if (player_direction == 3) {
            ssd1306_plot(63,1,1);
            ssd1306_plot(63,3,1);
        } else {
            ssd1306_plot(64,1,1);
            ssd1306_plot(64,3,1);
        }
    }
}
