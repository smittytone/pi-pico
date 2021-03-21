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
    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        Phantom* p = &phantoms[i];
        // Only move phantoms that are in the maze
        if (p->x != ERROR_CONDITION) {
            uint8_t old_x = p->x;
            uint8_t old_y = p->y;

            uint8_t new_x = p->x;
            uint8_t new_y = p->y;

            int8_t dx = p->x - player_x;
            int8_t dy = p->y - player_y;

            if (p->back_steps == 0) {
                // Make a standard move toward the player
                // Has the phantom caught the player?
                if (dx == 0 && dy == 0) {
                    game.in_play = false;
                    return;
                }

                if (dx > 0) {
                    --new_x;
                } else if (dx < 0) {
                    ++new_x;
                }

                if (dx != 0 && get_square_contents(new_x, old_y) != MAP_TILE_WALL) {
                    p->x = new_x;
                    p->direction = dx > 0 ? DIRECTION_EAST : DIRECTION_WEST;
                } else {
                    if (dy > 0) {
                        --new_y;
                    } else if (dy < 0) {
                        ++new_y;
                    }

                    if (dy != 0 && get_square_contents(old_x, new_y) != MAP_TILE_WALL) {
                        p->y = new_y;
                        p->direction = dy > 0 ? DIRECTION_SOUTH : DIRECTION_NORTH;
                    }
                }

                /*
                if (dx != 0 && dy != 0) {
                    // May move on either axis, but doesn't mean we can
                    uint8_t weight_x = false;
                    uint8_t weight_y = false;

                    // Calculate new squares
                    new_x += (dx > 0 ? 1 : -1);
                    new_y += (dy > 0 ? 1 : -1);

                    // Weight to favour moving in the direction of the biggest
                    // delta to the player -- should bring phantom to the
                    // player more quickly
                    if (abs(dx) > abs(dy)) ++weight_x;
                    if (abs(dy) > abs(dx)) ++weight_y;

                    // Big weight on whether Phantom *can* move
                    if (get_square_contents(new_x, old_y) == MAP_TILE_WALL) weight_x = 0;
                    if (get_square_contents(old_x, new_y) == MAP_TILE_WALL) weight_y = 0;

                    // All things equal? Pick a random direction
                    if (weight_x != 0 && weight_x == weight_y) {
                        if (irandom(0,100) > 50) {
                            weight_x++;
                        } else {
                            weight_y++;
                        }
                    }

                    // Apply the weights
                    if (weight_x > weight_y) {
                        p->x = new_x;
                    } else if (weight_x < weight_y) {
                        p->y = new_y;
                    }
                } else if (dy == 0) {
                    // On the same axis as the player, so move in the remaining axis
                    new_x += (dx > 0 ? 1 : -1);
                    if (get_square_contents(new_x, old_y) != MAP_TILE_WALL) {
                        p->x = new_x;
                        p->direction = dx > 0 ? DIRECTION_EAST : DIRECTION_WEST;
                    }
                } else {
                    // On the same axis as the player, so move in the remaining axis
                    new_y += (dy > 0 ? 1 : -1);
                    if (get_square_contents(old_x, new_y) != MAP_TILE_WALL) {
                        p->y = new_y;
                        p->direction = dy > 0 ? DIRECTION_SOUTH : DIRECTION_NORTH;
                    }
                }
                */
            } else {
                // Make a non-standard move in a random direction
                int8_t direction = irandom(0, 4);
                bool moved = false;
                while (!moved) {
                    switch(direction) {
                        case DIRECTION_NORTH:
                            if (p->y > 0 && get_square_contents(p->x, p->y - 1) != MAP_TILE_WALL) {
                                p->y -= 1;
                                moved = true;
                                break;
                            }
                        case DIRECTION_EAST:
                            if (p->y < 19 && get_square_contents(p->x, p->y + 1) != MAP_TILE_WALL) {
                                p->y += 1;
                                moved = true;
                                break;
                            }
                        case DIRECTION_SOUTH:
                            if (p->x < 19 && get_square_contents(p->x + 1, p->y) != MAP_TILE_WALL) {
                                p->x += 1;
                                moved = true;
                                break;
                            }
                        default:
                            if (p->x > 0 && get_square_contents(p->x - 1, p->y) != MAP_TILE_WALL) {
                                p->x -= 1;
                                moved = true;
                            }
                    }

                    direction++;
                    if (direction > DIRECTION_WEST) direction = DIRECTION_NORTH;
                }

                // Decrement the number of backwards steps
                p->back_steps--;
            }

            if (p->y == old_y && p->x == old_x && p->back_steps == 0) {
                // Phantom can't move towards player so move elsewhere
                // for 1-3 steps (assuming it isn't already)
                p->back_steps = irandom(1, 3);
            }
        }
    }
}


void move_phantoms2() {
    // Move each phantom toward the player
    for (uint8_t k = 0 ; k < game.phantoms ; ++k) {
        Phantom *p = &phantoms[k];
        // Only move phantoms that are in the maze
        if (p->x != ERROR_CONDITION) {
            uint8_t new_x = p->x;
            uint8_t new_y = p->y;

            // Get distance to player
            int8_t dx = p->x - player_x;
            int8_t dy = p->y - player_y;

            // Has the phantom got the player?
            if (dx == 0 && dy == 0) {
                // Yes!
                game.in_play = false;
                return;
            }

            uint8_t available_directions = 0;
            uint8_t favoured_directions = 0;
            uint8_t usable_directions = 0;

            // Determine the directions in which the phantom *can* move: empty spaces with no phantom already there
            if (p->x > 0 && get_square_contents(p->x - 1, p->y) != MAP_TILE_WALL && locate_phantom(p->x - 1, p->y) == ERROR_CONDITION) {
                available_directions |= PHANTOM_WEST;
            }

            if (p->x < 19 && get_square_contents(p->x + 1, p->y) != MAP_TILE_WALL && locate_phantom(p->x + 1, p->y) == ERROR_CONDITION) {
                available_directions |= PHANTOM_EAST;
            }

            if (p->y > 0 && get_square_contents(p->x, p->y - 1) != MAP_TILE_WALL && locate_phantom(p->x, p->y - 1) == ERROR_CONDITION) {
                available_directions |= PHANTOM_NORTH;
            }

            if (p->y < 19 && get_square_contents(p->x, p->y + 1) != MAP_TILE_WALL && locate_phantom(p->x, p->y + 1) == ERROR_CONDITION) {
                available_directions |= PHANTOM_WEST;
            }

            if (available_directions == 0) {
                // Phantom can't move anywhere -- all its exits are currently blocked
                return;
            }

            // Get move preferences
            if (dy > 0) favoured_directions |= PHANTOM_NORTH;
            if (dy < 0) favoured_directions |= PHANTOM_SOUTH;
            if (dx > 0) favoured_directions |= PHANTOM_WEST;
            if (dx < 0) favoured_directions |= PHANTOM_EAST;

            // Count up ways favoured moves and available squares match
            uint8_t count = 0;
            for (uint8_t i = 0 ; i < 4 ; ++i) {
                if ((available_directions & (i << i)) && (favoured_directions & (i << i))) {
                    // Phantom wants to go in a certain direction -- and has an exit
                    count++;
                    usable_directions |= (i << i);
                }
            }

            // Handle the move
            if (count == 1) {
                // Only one way to go, so take it
                move_one(usable_directions, &new_x, &new_y, k);
            } else if (count == 2) {
                // Two ways to go, so pick one at random
                uint8_t r = irandom(0,2);
                for (uint8_t i = 0 ; i < 4 ; ++i) {
                    if (usable_directions & (i << i)) {
                        if (r == 0) {
                            move_one((usable_directions & (i << i)), &new_x, &new_y, k);
                        } else {
                            r--;
                        }
                    }
                }
            } else {
                // Count == 0 -- special case where phantom can't move where it
                // wants so must move away or wait (if it has NOWHERE to go)
                if (available_directions != 0) {
                    for (uint8_t i = 0 ; i < 4 ; ++i) {
                        // Just pick the first available direction and take it
                        if (available_directions & (i << i)) {
                            move_one((available_directions & (i << i)), &new_x, &new_y, k);
                        }
                    }
                }
            }

            // Set the new location
            p->x = new_x;
            p->y = new_y;
        }
    }
}


void move_one(uint8_t c, uint8_t *x, uint8_t *y, uint8_t index) {
    // Refactored multiple-use code
    Phantom *p = &phantoms[index];
    if (c == PHANTOM_NORTH) *y = p->y - 1;
    if (c == PHANTOM_SOUTH) *y = p->y + 1;
    if (c == PHANTOM_EAST) *x = p->x + 1;
    if (c == PHANTOM_WEST) *x = p->x - 1;
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
            if (player_y == 0) return phantom;
            if (player_y - range < 0) range = player_y;
            for (uint8_t i = player_y ; i >= player_y - range ; --i) {
                phantom = locate_phantom(player_x, i);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        case DIRECTION_EAST:
            if (player_x == 19) return phantom;
            if (player_x + range > 19) range = 19 - player_x;
            for (uint8_t i = player_x ; i < player_y + range ; ++i) {
                phantom = locate_phantom(i, player_y);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        case DIRECTION_SOUTH:
            if (player_y == 19) return phantom;
            if (player_y + range > 19) range = 19 - player_y;
            for (uint8_t i = player_y ; i < player_y + range ; ++i) {
                phantom = locate_phantom(player_x, i);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        default:
            if (player_x == 0) return phantom;
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
                if (get_square_contents(player_x, i) != MAP_TILE_WALL) {
                    count += (locate_phantom(player_x, i) != ERROR_CONDITION ? 1 : 0);
                } else {
                    break;
                }
            }
            break;
        case DIRECTION_EAST:
            if (player_x + range > 19) range = 20 - player_x;
            for (uint8_t i = player_x ; i < player_y + range ; ++i) {
                if (get_square_contents(i, player_y) != MAP_TILE_WALL) {
                    count += (locate_phantom(i, player_y) != ERROR_CONDITION ? 1 : 0);
                } else {
                    break;
                }
            }
            break;
        case DIRECTION_SOUTH:
            if (player_y + range > 19) range = 20 - player_y;
            for (uint8_t i = player_y ; i < player_y + range ; ++i) {
                if (get_square_contents(player_x, i) != MAP_TILE_WALL) {
                    count += (locate_phantom(player_x, i) != ERROR_CONDITION ? 1 : 0);
                } else {
                    break;
                }
            }
            break;
        default:
            if (player_x - range < 0) range = player_x;
            for (uint8_t i = player_x ; i >= player_x - range ; --i) {
                if (get_square_contents(i, player_y) != MAP_TILE_WALL) {
                    count += (locate_phantom(i, player_y) != ERROR_CONDITION ? 1 : 0);
                } else {
                    break;
                }
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
