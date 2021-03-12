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
 * Define a basic map for testing
 */
char base_map_00[20] = "\xFF\xFF\xAA\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_01[20] = "\xFF\x00\x00\xFF\x00\x00\xFF\x00\x00\x00\x00\xFF\x00\x00\xFF\x00\x00\xFF\x00\x00";
char base_map_02[20] = "\xFF\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00";
char base_map_03[20] = "\xFF\xFF\xFF\x00\xFF\x00\xFF\x00\xFF\x00\x00\xFF\xFF\x00\x00\x00\x00\xFF\x00\xFF";
char base_map_04[20] = "\x00\x00\xFF\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\xFF\xFF\xFF";
char base_map_05[20] = "\xFF\xFF\xFF\xFF\xFF\x00\x00\xFF\x00\x00\xFF\x00\x00\x00\xFF\xFF\xFF\xFF\x00\x00";
char base_map_06[20] = "\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\xFF\xFF\xFF";
char base_map_07[20] = "\xFF\xFF\xFF\x00\x00\x00\x00\xFF\x00\xAA\x00\xFF\x00\x00\xFF\xFF\xFF\xFF\x00\x00";
char base_map_08[20] = "\xFF\x00\xFF\xFF\xFF\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\xFF\xFF\xFF";
char base_map_09[20] = "\xFF\xFF\xFF\x00\x00\xFF\xFF\x00\x00\x00\xFF\x00\x00\x00\x00\x00\x00\x00\xFF\x00";
char base_map_10[20] = "\x00\x00\xFF\xFF\x00\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF";
char base_map_11[20] = "\xFF\xFF\xFF\x00\xFF\xFF\xFF\xFF\x00\xFF\x00\x00\xFF\xFF\xFF\xFF\x00\xFF\x00\x00";
char base_map_12[20] = "\xFF\x00\xFF\xFF\xFF\x00\x00\xFF\xFF\xFF\xFF\x00\x00\x00\xFF\x00\x00\xFF\xFF\x00";
char base_map_13[20] = "\x00\x00\x00\x00\x00\x00\xFF\xFF\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\xFF";
char base_map_14[20] = "\x00\x00\x00\xFF\x00\x00\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\x00\x00\xFF\xFF\xFF";
char base_map_15[20] = "\x00\x00\x00\xFF\xFF\x00\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\xFF\xFF\x00\x00\x00";
char base_map_16[20] = "\xFF\x00\x00\xFF\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\xFF\x00\x00\x00\x00\x00";
char base_map_17[20] = "\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
char base_map_18[20] = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\x00\x00\xFF\x00\x00\xFF\x00\xFF\x00\xFF\x00";
char base_map_19[20] = "\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00";


/*
 * Map Functions
 */
void map_init() {
    // Load initial map
    // TODO Randomly generate a map
    current_map[0] = base_map_00;
    current_map[1] = base_map_01;
    current_map[2] = base_map_02;
    current_map[3] = base_map_03;
    current_map[4] = base_map_04;
    current_map[5] = base_map_05;
    current_map[6] = base_map_06;
    current_map[7] = base_map_07;
    current_map[8] = base_map_08;
    current_map[9] = base_map_09;
    current_map[10] = base_map_10;
    current_map[11] = base_map_11;
    current_map[12] = base_map_12;
    current_map[13] = base_map_13;
    current_map[14] = base_map_14;
    current_map[15] = base_map_15;
    current_map[16] = base_map_16;
    current_map[17] = base_map_17;
    current_map[18] = base_map_18;
    current_map[19] = base_map_19;
}


void show_map(uint8_t y_delta) {
    // Draw the current map on the screen buffer, centred but
    // vertically adjusted according to 'y_delta'

    // Put a Box around the map
    ssd1306_rect(40, 8 + y_delta, 48, 48, 1, false);

    // Draw the map
    uint8_t x = 44;
    uint8_t y = 12 + y_delta;

    for (uint8_t i = 0 ; i < 20 ; ++i) {
        char *line = current_map[i];
        for (uint8_t j = 0 ; j < 20 ; ++j) {
            uint8_t pixel = line[j];
            if (pixel != MAP_TILE_WALL) {
                ssd1306_plot(x + j * 2, y + i * 2, 1);
                ssd1306_plot(x + j * 2 + 1, y + i * 2, 1);
                ssd1306_plot(x + j * 2, y + i * 2 + 1, 1);
                ssd1306_plot(x + j * 2 + 1, y + i * 2 + 1, 1);
            }

            if (pixel == MAP_TILE_TELEPORTER) {
                ssd1306_plot(x + j * 2 + 1, y + i * 2, 0);
                ssd1306_plot(x + j * 2, y + i * 2 + 1, 0);
            }
        }
    }
}


uint8_t get_square_contents(uint8_t x, uint8_t y) {
    // Return the contents of the current map
    // specified grid reference
    if (x > 19 || y > 19) return MAP_TILE_WALL;
    char *line = current_map[y];
    return line[x];
}


bool set_square_contents(uint8_t x, uint8_t y, uint8_t value) {
    // Return the contents of the current map specified
    // grid reference and return true, otherwise false
    if (x > 19 || y > 19) return false;
    char *line = current_map[y];
    line[x] = value;
    return true;
}


uint8_t get_view_distance(uint8_t x, uint8_t y, uint8_t direction) {
    // Return the number of square the player can see
    // ahead of them in the direction they are facing
    uint8_t count = 0;
    if (x > 19 || y > 19 || direction > 3) return count;

    switch(direction) {
        case DIRECTION_NORTH:
            if (y == 0) return count;
            for (uint8_t i = y - 1 ; i >= 0 ; --i) {
                if (get_square_contents(x, i) != MAP_TILE_WALL) {
                    count++;
                } else {
                    break;
                }
            }
            break;
        case DIRECTION_EAST:
            for (uint8_t i = x + 1 ; i < 20 ; ++i) {
                if (get_square_contents(i, y) != MAP_TILE_WALL) {
                    count++;
                } else {
                    break;
                }
            }
            break;
        case DIRECTION_SOUTH:
            for (uint8_t i = y + 1 ; i < 20 ; ++i) {
                if (get_square_contents(x, i) != MAP_TILE_WALL) {
                    count++;
                } else {
                    break;
                }
            }
            break;
        default:
            if (x == 0) return count;
            for (uint8_t i = x - 1 ; i >= 0 ; --i) {
                if (get_square_contents(i, y) != MAP_TILE_WALL) {
                    count++;
                } else {
                    break;
                }
            }
    }

    if (count > 5) count = 5;
    return count;
}
