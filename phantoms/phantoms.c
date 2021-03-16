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
 * Basic Level Data
 */
uint8_t level_data[] = {
    1,1,1,2,1,3,1,4,1,5,1,6,2,6,2,6,2,6,3,6,3,6,3,6,4,6,4,6,4,6,4,8,4,8,4,8,5,9,5,9,5,9
};


/*
 * Phantom Control
 */
void move_phantoms() {
    // Move each phantom toward the player
    uint8_t phantom_spawns = 0;
    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        Phantom* p = &phantoms[i];
        if (p->x != ERROR_CONDITION) {
            uint8_t old_x = p->x;
            uint8_t old_y = p->y;
            int8_t dx = p->x - player_x;
            int8_t dy = p->y - player_y;

            if (p->rev == 0) {
                // Make a standard move
                // Caught the player?
                if (dx == 0 && dy == 0) {
                    game.in_play = false;
                    death();
                    break;
                }

                // Move the phantom in the x axis first
                if (dx > 0) {
                    p->x -= 1;
                } else if (dx < 0) {
                    p->x += 1;
                }

                // If we can't move in the x-axis, try the y-axis
                if (dx == 0 || get_square_contents(p->x, p->y) == MAP_TILE_WALL) {
                    p->x = old_x;

                    if (dy > 0) {
                        p->y -= 1;
                    } else if (dy < 0) {
                        p->y += 1;
                    }

                    if (dy == 0 || get_square_contents(p->x, p->y) == MAP_TILE_WALL) {
                        p->y = old_y;
                    } else {
                        p->direction = dy > 0 ? DIRECTION_SOUTH : DIRECTION_NORTH;
                    }
                } else {
                    p->direction = dx > 0 ? DIRECTION_EAST : DIRECTION_WEST;
                }
            } else {
                // Make a non-standard move
                if (p->y > 0 && get_square_contents(p->x, p->y - 1) != MAP_TILE_WALL) {
                    p->y += 1;
                } else if (p->y < 19  && get_square_contents(p->x, p->y + 1) != MAP_TILE_WALL) {
                    p->y += 1;
                } else if (p->x < 19  && get_square_contents(p->x + 1, p->y) != MAP_TILE_WALL) {
                    p->x += 1;
                } else if (p->x > 0 && get_square_contents(p->x - 1, p->y) != MAP_TILE_WALL) {
                    p->x -= 1;
                }

                --p->rev;
            }

            if (p->y == old_y && p->x == old_x) {
                // Phantom can't move towards player so move elsewhere
                // for 2-4 steps
                p->rev = irandom(2, 3);
            }
        } else {
            ++phantom_spawns;
        }
    }

    uint8_t count = 0;
    while (phantom_spawns > 0) {
        // Generate more phantoms if we need to
        Phantom* p = &phantoms[count];
        if (p->x == ERROR_CONDITION) {
            --phantom_spawns;
            while (true) {
                uint8_t x = irandom(0, 20);
                uint8_t y = irandom(0, 20);
                if (get_square_contents(x, y) == MAP_TILE_CLEAR) {
                    p->x = x;
                    p->y = y;
                    p->hp = game.level;
                    p->rev = 0;
                    break;
                }
            }
        }

        ++count;
    }
}


uint8_t get_facing_phantom(uint8_t range) {
    // Return the index of the closest facing phantom
    // in the 'phantoms' array -- or ERROR_CONDITION
    uint8_t phantom = ERROR_CONDITION;

    switch(player_direction) {
        case DIRECTION_NORTH:
            if (player_y - range < 0) range = player_y;
            for (uint8_t i = player_y ; i >= player_y - range ; --i) {
                phantom = locate_phantom(player_x, i);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        case DIRECTION_EAST:
            if (player_x + range > 19) range = 20 - player_x;
            for (uint8_t i = player_x ; i < player_y + range ; ++i) {
                phantom = locate_phantom(i, player_y);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        case DIRECTION_SOUTH:
            if (player_y + range > 19) range = 20 - player_y;
            for (uint8_t i = player_y ; i < player_y + range ; ++i) {
                phantom = locate_phantom(player_x, i);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        default:
            if (player_x - range < 0) range = player_x;
            for (uint8_t i = player_x ; i >= player_x - range ; --i) {
                phantom = locate_phantom(i, player_y);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
    }

    return phantom;
}


uint8_t locate_phantom(uint8_t x, uint8_t y) {
    // Return index of the phantom at (x,y) -- or ERROR_CONDITION
    for (uint8_t i = 0 ; i < game.phantoms ; i++) {
        Phantom p = phantoms[i];
        if (x == p.x && y == p.y) return i;
    }
    return ERROR_CONDITION;
}


void manage_phantoms() {

    if (game.level < 4) {
        if (game.level_kills == game.level) {
            ++game.level;
            ++game.phantoms;
        }
    } else {
        if (game.level_kills == 3) {
            ++game.level;
        }
    }

    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        Phantom *p = &phantoms[i];
        if (i < game.level && p->x == ERROR_CONDITION) {
            roll_new_phantom(i);
        }
    }

}

void roll_new_phantom(uint8_t index) {

    Phantom *p = &phantoms[index];
    uint8_t min = level_data[game.level - 1];
    uint8_t max = level_data[game.level];
    p->hp = irandom(min, max);
    p->hits = p->hp;
    p->rev = 0;
    p->direction = 0;

    while (true) {
        uint8_t x = irandom(0, 20);
        uint8_t y = irandom(0, 20);
        if (get_square_contents(x, y) == MAP_TILE_CLEAR && x != player_x && y != player_y) {
            p->x = x;
            p->y = y;
        }
    }
}