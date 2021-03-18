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
const uint8_t level_data[84] = {
    1,1,1,0,        // 1
    1,2,1,0,        // 2
    1,3,1,0,        // 3
    1,4,0,0,        // 4
    1,5,0,0,        // 5
    1,6,0,0,        // 6
    2,6,0,0,        // 7
    2,6,0,0,        // 8
    2,6,0,0,        // 9
    3,6,0,1,        // 10
    3,6,0,1,        // 11
    3,6,0,1,        // 12
    4,6,0,1,        // 13
    4,6,0,1,        // 14
    4,6,0,1,        // 15
    4,8,0,2,        // 16
    4,8,0,2,        // 17
    4,8,0,2,        // 18
    5,9,0,2,        // 19
    5,9,0,2,        // 20
    5,9,0,2         // 21
};


/*
 * Phantom Control
 */
void move_phantoms() {
    // Move each phantom toward the player
    uint8_t phantom_spawns = 0;
    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        Phantom* p = &phantoms[i];
        // Only move phantoms that are in the maze
        if (p->x != ERROR_CONDITION) {
            uint8_t old_x = p->x;
            uint8_t old_y = p->y;
            int8_t dx = p->x - player_x;
            int8_t dy = p->y - player_y;

            if (p->back_steps == 0) {
                // Make a standard move toward the player
                // Has the phantom caught the player?
                if (dx == 0 && dy == 0) {
                    game.in_play = false;
                    return;
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
                // Make a non-standard move in a random direction
                int8_t direction = irandom(0, 4);
                switch(direction) {
                    case 0:
                        if (p->y > 0 && get_square_contents(p->x, p->y - 1) != MAP_TILE_WALL) {
                            p->y -= 1;
                            break;
                        }
                    case 1:
                        if (p->y < 19 && get_square_contents(p->x, p->y + 1) != MAP_TILE_WALL) {
                            p->y += 1;
                            break;
                        }
                    case 2:
                        if (p->x < 19 && get_square_contents(p->x + 1, p->y) != MAP_TILE_WALL) {
                            p->x += 1;
                            break;
                        }
                    default:
                        if (p->x > 0 && get_square_contents(p->x - 1, p->y) != MAP_TILE_WALL) {
                            p->x -= 1;
                        }
                }

                // Decrement the number of non-standard steps
                --p->back_steps;
            }

            if (p->y == old_y && p->x == old_x) {
                // Phantom can't move towards player so move elsewhere
                // for 4-9 steps
                p->back_steps = irandom(4, 5);
            }
        }
    }
}


void manage_phantoms() {
    // Check whether we need to increase the number of phantoms
    // on the board or increase their speed -- all caused by a
    // level-up. We up the level if all the level's phantoms have
    // been zapped
    bool level_up = false;

    if (game.level < MAX_PHANTOMS) {
        if (game.level_kills == game.level) {
            ++game.level;
            level_up = true;
            game.level_kills = 0;
            ++game.phantoms;
        }
    } else {
        if (game.level_kills == MAX_PHANTOMS) {
            ++game.level;
            level_up = true;
            game.level_kills = 0;
        }
    }

    // Just in case...
    if (game.phantoms > MAX_PHANTOMS) game.phantoms = MAX_PHANTOMS;

    // Did we level-up? Is so, set the phantom movement speed
    if (level_up) {
        uint8_t index = (game.level - 1) * 4;
        game.phantom_speed = ((PHANTOM_MOVE_TIME_US << level_data[index + 2]) >> level_data[index + 3]);
    }

    // Do we need to add any new phantoms to the board?
    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        if (phantoms[i].x == ERROR_CONDITION) roll_new_phantom(i);
    }
}


void roll_new_phantom(uint8_t phantom_index) {
    // Generate a new phantom at the specified index of the
    // 'phantoms' data array
    if (phantom_index > MAX_PHANTOMS - 1) return;
    Phantom *p = &phantoms[phantom_index];
    uint8_t level_index = (game.level - 1) * 4;
    uint8_t min = level_data[level_index];
    uint8_t max = level_data[level_index + 1];
    p->hp = irandom(min, max);
    p->hp_max = p->hp;
    p->back_steps = 0;
    p->direction = 0;

    // Place the phantom randomly at any empty
    // square on the board
    while (true) {
        uint8_t x = irandom(0, 20);
        uint8_t y = irandom(0, 20);
        if (get_square_contents(x, y) == MAP_TILE_CLEAR && x != player_x && y != player_y) {
            p->x = x;
            p->y = y;
            break;
        }
    }
}


uint8_t get_facing_phantom(uint8_t range) {
    // Return the index of the closest facing phantom
    // in the 'phantoms' array -- or ERROR_CONDITION.
    // 'range' is the number of squares we'll iterate
    // over
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


uint8_t count_facing_phantoms(uint8_t range) {
    // Return the index of the closest facing phantom
    // in the 'phantoms' array -- or ERROR_CONDITION.
    // 'range' is the number of squares we'll iterate
    // over
    uint8_t count = 0;

    switch(player_direction) {
        case DIRECTION_NORTH:
            if (player_y - range < 0) range = player_y;
            for (uint8_t i = player_y ; i >= player_y - range ; --i) {
                count += (locate_phantom(player_x, i) > 0 ? 1 : 0);
            }
            break;
        case DIRECTION_EAST:
            if (player_x + range > 19) range = 20 - player_x;
            for (uint8_t i = player_x ; i < player_y + range ; ++i) {
                count += (locate_phantom(player_x, i) > 0 ? 1 : 0);
            }
            break;
        case DIRECTION_SOUTH:
            if (player_y + range > 19) range = 20 - player_y;
            for (uint8_t i = player_y ; i < player_y + range ; ++i) {
                count += (locate_phantom(player_x, i) > 0 ? 1 : 0);
            }
            break;
        default:
            if (player_x - range < 0) range = player_x;
            for (uint8_t i = player_x ; i >= player_x - range ; --i) {
                count += (locate_phantom(player_x, i) > 0 ? 1 : 0);
            }
            break;
    }

    return count;
}


uint8_t locate_phantom(uint8_t x, uint8_t y) {
    // Return the index of the phantom at (x,y) -- or ERROR_CONDITION
    // of there is no phantom at that location
    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        if (x == phantoms[i].x && y == phantoms[i].y) return i;
    }
    return ERROR_CONDITION;
}
