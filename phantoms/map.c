#include "main.h"


/*
 * Map Functions
 */
void map_init() {
    // Load initial map
    // TODO Randomly generate a map
    current_map[0] = base_map_01;
    current_map[1] = base_map_02;
    current_map[2] = base_map_03;
    current_map[3] = base_map_04;
    current_map[4] = base_map_05;
    current_map[5] = base_map_06;
    current_map[6] = base_map_07;
    current_map[7] = base_map_08;
    current_map[8] = base_map_09;
    current_map[9] = base_map_10;
    current_map[10] = base_map_11;
    current_map[11] = base_map_12;
    current_map[12] = base_map_13;
    current_map[13] = base_map_14;
    current_map[14] = base_map_15;
    current_map[15] = base_map_16;
    current_map[16] = base_map_17;
    current_map[17] = base_map_18;
    current_map[18] = base_map_19;
    current_map[19] = base_map_20;
}


void set_test_map() {

}


void show_map(uint8_t y_delta) {
    // Draw the current map on the screen buffer

    // Put a Box around the map
    ssd1306_rect(40, 8 + y_delta, 48, 48, 1, false);

    // Draw the map
    uint8_t x = 44;
    uint8_t y = 12 + y_delta;

    for (uint8_t i = 0 ; i < 20 ; ++i) {
        char *line = current_map[i];
        for (uint8_t j = 0 ; j < 20 ; ++j) {
            uint8_t pixel = line[j];
            if (pixel == 0xFF) {
                ssd1306_plot(x + j * 2, y + i * 2, 1);
                ssd1306_plot(x + j * 2 + 1, y + i * 2, 1);
                ssd1306_plot(x + j * 2, y + i * 2 + 1, 1);
                ssd1306_plot(x + j * 2 + 1, y + i * 2 + 1, 1);
            }
        }
    }
}


uint8_t get_square_contents(uint8_t x, uint8_t y) {
    // Return the contents of the current map
    // specified grid reference
    char *line = current_map[y];
    return line[x];
}


uint8_t get_view_distance(uint8_t x, uint8_t y, uint8_t direction) {
    // Return the number of square the player can see
    // ahead of them in the directio they are facing
    uint8_t count = 0;
    if (x > 19 || y > 19 || direction > 3) return count;

    switch(direction) {
        case DIRECTION_NORTH:
            for (uint8_t i = y ; i >= 0 ; --i) {
                if (i == 0) break;
                if (get_square_contents(x, i) != 0 ) {
                    count++;
                } else {
                    break;
                }
            }
            break;
        case DIRECTION_EAST:
            for (uint8_t i = x ; i < 20 ; ++i) {
                if (i == 19) break;
                if (get_square_contents(i, y) != 0) {
                    count++;
                } else {
                    break;
                }
            }
            break;
        case DIRECTION_SOUTH:
            for (uint8_t i = y ; i < 20 ; ++i) {
                if (i == 19) break;
                if (get_square_contents(x, i) != 0) {
                    count++;
                } else {
                    break;
                }
            }
            break;
        default:
            for (uint8_t i = x ; i >= 0 ; --i) {
                if (i == 0) break;
                if (get_square_contents(i, y) != 0) {
                    count++;
                } else {
                    break;
                }
            }
    }

    if (count > 6) count = 6;
    return count;
}
