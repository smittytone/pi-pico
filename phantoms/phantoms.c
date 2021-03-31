/*
 * Phantoms
 *
 * @version     1.0.1
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
    for (uint8_t k = 0 ; k < game.phantoms ; ++k) {
        Phantom *p = &phantoms[k];
        // Only move phantoms that are in the maze
        if (p->x != ERROR_CONDITION) {
            uint8_t new_x = p->x;
            uint8_t new_y = p->y;
            uint8_t new_direction = p->direction;

            // Get distance to player
            int8_t dx = p->x - player_x;
            int8_t dy = p->y - player_y;

            // Has the phantom got the player?
            if (dx == 0 && dy == 0) {
                // Yes!
                game.in_play = false;
                return;
            }

            // Set up direction storage
            uint8_t available_directions = 0;
            uint8_t favoured_directions = 0;
            uint8_t usable_directions = 0;
            uint8_t exit_count = 0;

            // Determine the directions in which the phantom *can* move: empty spaces with no phantom already there
            if (p->x > 0 && get_square_contents(p->x - 1, p->y) != MAP_TILE_WALL && locate_phantom(p->x - 1, p->y) == ERROR_CONDITION) {
                available_directions |= PHANTOM_WEST;
                ++exit_count;
            }

            if (p->x < 19 && get_square_contents(p->x + 1, p->y) != MAP_TILE_WALL && locate_phantom(p->x + 1, p->y) == ERROR_CONDITION) {
                available_directions |= PHANTOM_EAST;
                ++exit_count;
            }

            if (p->y > 0 && get_square_contents(p->x, p->y - 1) != MAP_TILE_WALL && locate_phantom(p->x, p->y - 1) == ERROR_CONDITION) {
                available_directions |= PHANTOM_NORTH;
                ++exit_count;
            }

            if (p->y < 19 && get_square_contents(p->x, p->y + 1) != MAP_TILE_WALL && locate_phantom(p->x, p->y + 1) == ERROR_CONDITION) {
                available_directions |= PHANTOM_SOUTH;
                ++exit_count;
            }

            if (available_directions == 0) {
                // Phantom can't move anywhere -- all its exits are currently blocked
                return;
            }

            // FROM 1.0.1
            // Move away from the player if the Phantom is reversing
            uint8_t from = 0;
            if (p->back_steps > 0) {
                // Phantom is indeed reversing, so get the direction
                // from which it moved into this square -- we'll use this
                // to prevent the Phantom from back-tracking if in the next
                // lines it can go after the player again
                from = get_phantom_direction(k);
                if (exit_count > 2) {
                    // The Phantom has reached a junction, ie. a square with more than
                    // two exits, so reset the reversal and try to move toward the player again
                    p->back_steps = 0;
                } else {
                    // The Phantom isn't at a junction, so calculate its
                    // vector away from the player ('dx' and 'dy'
                    // vector toward the player unless changed here)
                    dx *= -1;
                    dy *= -1;
                }
            }

            // Get the Phantom's move preferences -- the direction(s)
            // in which it would like to go
            if (dy > 0) favoured_directions |= PHANTOM_NORTH;
            if (dy < 0) favoured_directions |= PHANTOM_SOUTH;
            if (dx > 0) favoured_directions |= PHANTOM_WEST;
            if (dx < 0) favoured_directions |= PHANTOM_EAST;

            // FROM 1.0.1
            // Remove the way the Phantom has come from its list of
            // favoured directions. This is used when it's reversing
            // and has reached a junction (ie. 'from != 0')
            favoured_directions &= (~from);

            // Count up the number of ways favoured moves and available squares match
            uint8_t count = 0;
            for (uint8_t i = 0 ; i < 4 ; ++i) {
                if ((available_directions & (1 << i)) && (favoured_directions & (1 << i))) {
                    // Phantom wants to go in a certain direction and it has an exit
                    // so record this as a usable direction
                    count++;
                    usable_directions |= (1 << i);
                }
            }

            // Handle the move itself
            if (count == 1) {
                // Only one way for the Phantom to go, so take it
                move_one(usable_directions, &new_x, &new_y, k);
                new_direction = usable_directions;
            } else if (count == 2) {
                // The Phantom has two ways to go, so pick one of them at random:
                // even roll go the first way (0); odd roll go the second (1)
                uint8_t r = (irandom(1, 100) % 2);
                uint8_t i = 0;
                while(true) {
                    // Iterate through the directions until we have that can be
                    // used and then is selectable (r == 0)
                    if (usable_directions & (1 << i)) {
                        if (r == 0) {
                            // Take this direction
                            move_one((usable_directions & (1 << i)), &new_x, &new_y, k);
                            new_direction = (usable_directions & (1 << i));
                            break;
                        } else {
                            // Ignore this direction
                            r--;
                        }
                    }

                    ++i;
                    if (i > 3) i = 0;
                }
            } else {
                // Count == 0 -- this is the special case where phantom can't move
                //  where it wants so must move away or wait (if it has NOWHERE to go)
                if (available_directions != 0) {
                    // Just pick a random available direction and take it,
                    // but remove the way the phantom came (provided it
                    // doesn't leave it with no way out)
                    // NOTE re-calculate 'from' here so we don't mess up
                    //      the ealier case when it needs to be zero
                    from = get_phantom_direction(k);
                    uint8_t ad = available_directions;
                    ad &= (~from);

                    // Just in case removing from leaves the Phantom nowhere to go
                    // ie. it's at a dead end
                    if (ad != 0) available_directions = ad;

                    // Pick a random value and count down through the available exits
                    // until it comes to zero -- then take that one
                    uint8_t i = 0;
                    uint8_t r = irandom(0, 4);
                    while (true) {
                        if ((available_directions & (1 << i)) > 0) {
                            if (r == 0) {
                                move_one((available_directions & (1 << i)), &new_x, &new_y, k);
                                new_direction = (available_directions & (1 << i));
                                p->back_steps = 1;
                                break;
                            } else {
                                r--;
                            }
                        }

                        ++i;
                        if (i > 3) i = 0;
                    }
                }
            }

            // Set the Phantom's new location
            p->x = new_x;
            p->y = new_y;
            p->direction = new_direction;
        }
    }
}


void move_one(uint8_t direction, uint8_t *x, uint8_t *y, uint8_t phantom_index) {
    // Refactored multiple-use code: move the Phantom one
    // space according in the chosen direction
    Phantom *p = &phantoms[phantom_index];
    if (direction == PHANTOM_NORTH) *y = p->y - 1;
    if (direction == PHANTOM_SOUTH) *y = p->y + 1;
    if (direction == PHANTOM_EAST)  *x = p->x + 1;
    if (direction == PHANTOM_WEST)  *x = p->x - 1;
}


uint8_t get_phantom_direction(uint8_t phantom_index) {
    // Return the direction the phantom has come from
    Phantom *p = &phantoms[phantom_index];
    if (p->direction == PHANTOM_WEST)  return PHANTOM_EAST;
    if (p->direction == PHANTOM_EAST)  return PHANTOM_WEST;
    if (p->direction == PHANTOM_NORTH) return PHANTOM_SOUTH;
    return PHANTOM_NORTH;
}


void manage_phantoms() {
    // Check whether we need to increase the number of phantoms
    // on the board or increase their speed -- all caused by a
    // level-up. We up the level if all the level's phantoms have
    // been zapped
    bool level_up = false;

    // If we're on levels 1 and 2, we only have that number of
    // Phantoms. From 3 and up, there are aways three in the maze
    if (game.level < MAX_PHANTOMS) {
        if (game.level_kills == game.level) {
            game.level_kills = 0;
            ++game.level;
            ++game.phantoms;
            level_up = true;
        }
    } else {
        if (game.level_kills == MAX_PHANTOMS) {
            game.level_kills = 0;
            level_up = true;
            ++game.level;
        }
    }

    // Did we level-up? Is so, update the phantom movement speed
    if (level_up) {
        uint8_t index = (game.level - 1) * 4;
        game.phantom_speed = ((PHANTOM_MOVE_TIME_US << level_data[index + 2]) >> level_data[index + 3]);
    }

    // Just in case...
    if (game.phantoms > MAX_PHANTOMS) game.phantoms = MAX_PHANTOMS;

    // Do we need to add any new phantoms to the board?
    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        // A Phantom is off the board, so put it back on
        if (phantoms[i].x == ERROR_CONDITION) roll_new_phantom(i);
    }
}


void roll_new_phantom(uint8_t phantom_index) {
    // Generate a new phantom at the specified index of the
    // 'phantoms' data array
    if (phantom_index > MAX_PHANTOMS - 1) return;
    Phantom *p = &phantoms[phantom_index];
    uint8_t level_index = (game.level - 1) * 4;
    uint8_t min_hit_points = level_data[level_index];
    uint8_t max_hit_points = level_data[level_index + 1];
    p->hp = irandom(min_hit_points, max_hit_points);
    p->hp_max = p->hp;
    p->back_steps = 0;
    p->direction = 0;

    // Place the phantom randomly at any empty square on the board
    while (true) {
        uint8_t x = irandom(0, 20);
        uint8_t y = irandom(0, 20);

        // UPDATED 1.0.1
        // Make sure we're selecting a clear square, the player is not there
        // already and is not in an adjacent square either
        bool good = (get_square_contents(x, y) == MAP_TILE_CLEAR);
        good &= ((x != player_x)     && (y != player_y));
        good &= ((x != player_x - 1) && (x != player_x + 1));
        good &= ((y != player_y - 1) && (y != player_y + 1));

        if (good) {
            p->x = x;
            p->y = y;
            break;
        }
    }
}


uint8_t get_facing_phantom(uint8_t range) {
    // Return the index of the closest facing Phantom to the
    // player from the the 'phantoms' array -- or ERROR_CONDITION.
    // 'range' is the number of squares we'll iterate over
    uint8_t phantom = ERROR_CONDITION;

    switch(player_direction) {
        case DIRECTION_NORTH:
            if (player_y == 0) return phantom;
            if (player_y - range < 0) range = player_y;
            for (int8_t i = player_y ; i > player_y - range ; --i) {
                phantom = locate_phantom(player_x, i);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        case DIRECTION_EAST:
            if (player_x == 19) return phantom;
            if (player_x + range > 19) range = 19 - player_x;
            for (int8_t i = player_x ; i < player_x + range ; ++i) {
                phantom = locate_phantom(i, player_y);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        case DIRECTION_SOUTH:
            if (player_y == 19) return phantom;
            if (player_y + range > 19) range = 19 - player_y;
            for (int8_t i = player_y ; i < player_y + range ; ++i) {
                phantom = locate_phantom(player_x, i);
                if (phantom != ERROR_CONDITION) return phantom;
            }
            break;
        default:
            if (player_x == 0) return phantom;
            if (player_x - range < 0) range = player_x;
            for (int8_t i = player_x ; i > player_x - range ; --i) {
                phantom = locate_phantom(i, player_y);
                if (phantom != ERROR_CONDITION) return phantom;
            }
    }

    return phantom;
}


uint8_t count_facing_phantoms(uint8_t range) {
    // Return the number of Phantoms in front of the player
    // 'range' is the number of squares we'll iterate over
    uint8_t phantom_count = 0;

    switch(player_direction) {
        case DIRECTION_NORTH:
            if (player_y == 0) return phantom_count;
            if (player_y - range < 0) range = player_y;
            for (int8_t i = player_y ; i >= player_y - range ; --i) {
                phantom_count += (locate_phantom(player_x, i) != ERROR_CONDITION ? 1 : 0);;
            }
            break;
        case DIRECTION_EAST:
            if (player_x == 19) return phantom_count;
            if (player_x + range > 19) range = 19 - player_x;
            for (int8_t i = player_x ; i <= player_x + range ; ++i) {
                phantom_count += (locate_phantom(i, player_y) != ERROR_CONDITION ? 1 : 0);;
            }
            break;
        case DIRECTION_SOUTH:
            if (player_y == 19) return phantom_count;
            if (player_y + range > 19) range = 19 - player_y;
            for (int8_t i = player_y ; i <= player_y + range ; ++i) {
                phantom_count += (locate_phantom(player_x, i) != ERROR_CONDITION ? 1 : 0);;
            }
            break;
        default:
            if (player_x == 0) return phantom_count;
            if (player_x - range < 0) range = player_x;
            for (int8_t i = player_x ; i >= player_x - range ; --i) {
                phantom_count += (locate_phantom(i, player_y) != ERROR_CONDITION ? 1 : 0);;
            }
    }

    return phantom_count;
}


uint8_t locate_phantom(uint8_t x, uint8_t y) {
    // Return the index of the phantom at (x,y) -- or ERROR_CONDITION
    // if there is NO phantom at that location
    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        if (x == phantoms[i].x && y == phantoms[i].y) return i;
    }
    return ERROR_CONDITION;
}
