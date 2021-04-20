/*
 * Planet Invasion for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#include "invasion.h"


void move_crystals() {
    // Are any crystals falling?
    uint32_t now = time_us_32();
    if (now - timers[TIMER_CRYSTAL_MOVES] > CRYSTAL_FALL_SPEED) {
        for (uint8_t i = 0 ; i < wave.number[CRYSTAL_INDEX] ; ++i) {
            Crystal *c = wave.crystals[i];
            // If the crystal is taken, on the ground or being held,
            // we don't need to move it
            if ((c->x == CRYSTAL_TAKEN) || (c->y == GROUND_LEVEL) || c->held) continue;

            c->y += 1;
            if (c->y >= GROUND_LEVEL) {
                c->y = GROUND_LEVEL;

                // It has hit the ground -- did it survive?
                if (c->drop_height > 20) {
                    // No!
                    c->x = CRYSTAL_TAKEN;
                } else {
                    // Yes! Assign points...
                    game.score += SCORE_DROP_CRYSTAL;
                }
            }
        }

        // Retain the current time
        timers[TIMER_CRYSTAL_MOVES] = now;
    }
}


uint8_t live_crystal_count() {
    // Return the number of crystals currently in the game
    uint8_t count = 0;
    for (uint8_t i = 0 ; i < wave.number[CRYSTAL_INDEX] ; ++i) {
        Crystal *c = wave.crystals[i];
        if (c->x != CRYSTAL_TAKEN) ++count;
    }
    return count;
}


uint8_t closest_crystal(int16_t x) {
    // Find the closest crystal to the specified x-axis co-ordinate
    uint8_t closest = 255;
    uint8_t choice = 255;
    for (uint8_t i = 0 ; i < wave.number[CRYSTAL_INDEX] ; ++i) {
        Crystal *c = wave.crystals[i];
        if (c->x == CRYSTAL_TAKEN) continue;
        uint8_t distance = abs(c->x - x);
        if (distance < closest) {
            closest = distance;
            choice = i;
        }
    }

    return (closest > 32 ? 255: choice);
}


void move_bullets() {
    // Move 'live' bullets onward
    // NOTE bullets array values are:
    //      i     - x co-ordinate
    //      i + 1 - y co-ordinate
    //      i + 2 - x delta
    //      i + 3 - y delta
    uint32_t now = time_us_32();
    if (now - timers[TIMER_BULLET_MOVES] > BULLET_SPEED) {
        for (uint8_t i = 0 ; i < MAX_BULLETS * 4 ; i += 4) {
            if (bullets[i] == NO_CRYSTAL) continue;

            // Move the bullet
            bullets[i]     += bullets[i + 2];
            bullets[i + 1] += bullets[i + 3];

            // If it's now off the screen, clear the record
            if ((bullets[i + 1] > GROUND_LEVEL) || (bullets[i + 1] < 1)) {
                bullets[i] = NO_CRYSTAL;
                continue;
            }

            // Handle the x-axis screen wrap
            if (bullets[i] >= PLAY_FIELD_WIDTH) bullets[i] -= PLAY_FIELD_WIDTH;
            if (bullets[i] < 0) bullets[i] += PLAY_FIELD_WIDTH;
        }

        // Retain the current time
        timers[TIMER_BULLET_MOVES] = now;
    }
}


bool fire_bullet(int16_t x, int8_t y) {
    // Fire a bullet if there are any available
    for (uint8_t i = 0 ; i < MAX_BULLETS * 4 ; i += 4) {
        if (bullets[i] != NO_CRYSTAL) continue;

        // Set off the bullet
        bullets[i] = x + 4;
        if (x + 4 >= PLAY_FIELD_WIDTH) bullets[i] = x + 4 - PLAY_FIELD_WIDTH;
        if (x + 4 < 0) bullets[i] = PLAY_FIELD_WIDTH - (x + 4);

        bullets[i + 1] = y;

        // Get the x direction to the player
        if (x - player.x == 0) {
            bullets[i + 2] = 0;
        } else {
            bullets[i + 2] = (x - player.x < 0) ? 1 : -1;
            bullets[i + 2] *= irandom(1,2);
        }

        if (y - player.y == 0) {
            bullets[i + 3] = 0;
        } else {
            bullets[i + 3] = (y - player.y < 0) ? 1 : -1;
            bullets[i + 3] *= irandom(1,2);
        }

        // Only one bullet per Invader
        return true;
    }

    return false;
}
