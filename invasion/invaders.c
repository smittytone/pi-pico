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


uint8_t maxes[8] = {MAX_GRABBERS, MAX_GRABBERS, MAX_CHASERS, MAX_BERSERKERS, MAX_BEAMERS, MAX_MINERS, MAX_MINES, MAX_CRYSTALS};


/*
 * GENERIC
 */
void move_monsters() {
    // Cycle though the various invader movement routines every
    // 'INVADER_BASE_MOVE_SPEEED' secoonds (0.5)
    uint32_t now = time_us_32();
    if (now - timers[TIMER_INVADER_MOVES_SLOW] > INVADER_BASE_MOVE_SPEEED) {
        move_grabbers();
        move_miners();
        move_beamers();
        // move_chasers();
        timers[TIMER_INVADER_MOVES_SLOW] = now;
    }

    if (now - timers[TIMER_INVADER_MOVES_QUICK] > INVADER_FAST_MOVE_SPEEED) {
        move_killers();
        move_berserkers();
        // move_chasers();
        timers[TIMER_INVADER_MOVES_QUICK] = now;
    }
}


uint8_t live_monster_count() {
    // Return the number of monsters in the game that count
    // toward the number of kills needed to clear a wave:
    // Grabbers, Miners, Beamers
    uint8_t count = 0;
    count += count_invader(INVADER_GRABBER);
    count += count_invader(INVADER_MINER);
    count += count_invader(INVADER_BEAMER);
    count += count_invader(INVADER_BERSERKER);
    return count;
}


uint8_t count_invader(uint8_t kind) {
    uint8_t count = 0;

    for (uint8_t i = 0 ; i < maxes[kind] ; ++i) {
        if (wave.invaders[kind][i] == NULL) continue;
        Invader *k = wave.invaders[kind][i];
        if (k->x != INVADER_KILLED) ++count;
    }

    return count;
}


uint8_t get_max_invaders(uint8_t kind) {
    return maxes[kind];
}


void free_invaders(uint8_t kind) {
    // Clear the memory of any invader struct by type
    for (uint8_t i = 0 ; i < maxes[kind] ; ++i) {
        if (wave.invaders[kind][i] != NULL) {
            free(wave.invaders[kind][i]);
            wave.invaders[kind][i] = NULL;
        }
    }

    wave.number[kind] = 0;
}


int8_t new_invader(uint8_t kind) {
    // Generic creator function for invaders which
    // spawn during a wave, not at the start of a wave
    // Return the index in the specific array storing
    // the created type of invader
    for (uint8_t i = 0 ; i < maxes[kind] ; ++i) {
        if (wave.invaders[kind][i] == NULL) {
            Invader *m = alloc(sizeof(Invader));
            wave.invaders[kind][i] = m;
            return i;
        } else {
            Invader *k = wave.invaders[kind][i];
            if (k->x == INVADER_KILLED) return i;
        }
    }

    // Error condition
    return INVADER_KILLED;
}


int8_t check_y(int8_t y) {
    // Make sure a new y-axis co-ordinate is valid
    if (y < 1) y = 1;
    if (y > 55) y = 55;
    return y;
}


int16_t check_x(int16_t x) {
    // Make sure a new x-axis co-ordinate is valid
    if (x < 0) x += PLAY_FIELD_WIDTH;
    if (x >= PLAY_FIELD_WIDTH) x -= PLAY_FIELD_WIDTH;
    return x;
}


/*
 * GRABBERS
 */
void init_grabbers(uint8_t number) {
    // Roll a new set of Grabbers, clearing any existing ones first
    // This should only take place once per wave
    free_invaders(INVADER_GRABBER);

    // Determine the number of Grabbers in the wave
    number += irandom(1, 5);
    if (number > MAX_GRABBERS) number = MAX_GRABBERS;
    wave.number[INVADER_GRABBER] = number;

    // Roll the Grabbers
    for (uint8_t i = 0 ; i < number ; ++i) {
        Invader *g = alloc(sizeof(Invader));
        g->crystal = NO_CRYSTAL;
        g->type = INVADER_GRABBER;
        wave.grabbers[i] = g;

        // Place the Grabber
        bool placed = false;
        do {
            placed = true;
            g->x = irandom(10, 622);
            g->y = irandom(5, 40);

            // Make sure they're not too close together at the start
            if (i > 1) {
                for (uint8_t j = 0 ; j < i ; ++j) {
                    Invader *a = wave.grabbers[j];
                    if (abs(g->x - a->x) < 19) {
                        placed = false;
                        break;
                    }
                }
            }
        } while(!placed);
    }
}


void move_grabbers() {
    // Move the Grabbers
    uint8_t grabbed = 0;
    for (uint8_t i = 0 ; i < MAX_GRABBERS ; ++i) {
        if (wave.grabbers[i] == NULL) continue;

        Invader *g = wave.grabbers[i];
        if (g->x == INVADER_KILLED) continue;

        // A Grabber moves slowly down the screen to grab a crystal
        // May make it move more quickly upwards TODO
        if (g->crystal == NO_CRYSTAL) {
            // Calculate the x-drift
            uint8_t closest = closest_crystal(g->x);
            uint8_t height  = GROUND_LEVEL - g->y;
            int8_t lateral  = 0;
            Crystal *c;

            if (closest == 255) {
                // No nearby crystal
                int16_t dx = player.x - g->x;
                int8_t  dy = player.y - g->y;

                if (dx < 0) dx = (irandom(0,2) == 0) ? -2 : -1;
                if (dx > 0) dx = (irandom(0,2) == 0) ? 2 : 1;
                g->x = check_x(g->x + dx);

                if (dy == 0 && g->dy == 0) {
                    // Grabber moving laterally, so add some jink
                    g->dy = irandom(0,2) - 1;
                    g->y = check_y(g->y + g->dy);
                } else {
                    check_y(g->y + (dy > 0 ? -1 : 1));
                }
            } else {
                // Crystal in range -- go straight for it
                c = wave.crystals[closest];
                if (c->x - g->x != 0) lateral = (c->x - g->x < 0) ? -2 : 2;
                g->x = check_x(g->x + lateral);
                g->y += 1;
            }

            // Has the Grabber reached a crystal?
            if (g->y > 52) {
                g->y = 52;
                if ((closest != 255) && (c->x >= g->x - 2) && (c->x <= g->x + 10)) {
                    // Got one
                    g->crystal = closest;
                    c->held = true;
                }
            }
        } else {
            // Go straight up
            g->y -= 2;
            Crystal *c = wave.crystals[g->crystal];
            c->x = g->x + 7;
            c->y = g->y + 8;
            ++grabbed;
            if (grabbed < 2) tone(100, 20, 0);
        }

        // Will the Grabber fire?
        if ((g->crystal == NO_CRYSTAL) && (irandom(0,100) > (70 - game.level))) {
            fire_bullet(g->x, g->y);
        }

        // Has the Grabber reached the top?
        if ((g->y < 1) && (g->crystal >= 0)) {
            // GRABBER BECOMES A KILLER
            new_killer(g->x);
            g->x = CRYSTAL_TAKEN;
            free(wave.grabbers[i]);
            wave.grabbers[i] = NULL;
        }
    }
}


/*
 * MINERS
 */
void init_miners() {
    // Roll a new set of Miners, clearing any existing ones first
    // This should only take place once per wave
    if (game.level > 1) {
        free_invaders(INVADER_MINER);

        // Determine the number of Miners in the wave
        uint8_t n = 0;
        if (game.level % 2 == 0) {
            n = ((game.level >> 1) + 1);
        } else {
            n = (((game.level - 1) >> 1) + 1);
        }

        if (n > MAX_MINERS) n = MAX_MINERS;
        wave.number[INVADER_MINER] = n;

        // Roll the Miners
        for (uint8_t i = 0 ; i < n ; ++i) {
            Invader *m = alloc(sizeof(Invader));
            wave.miners[i] = m;
            m->type = INVADER_MINER;
            m->x = irandom(10, 622);
            m->y = irandom(20, 30);
            m->last_mine_x = -1;
        }
    }
}


void move_miners() {
    if (wave.number[INVADER_MINER]  > 0){
        for (uint8_t i = 0 ; i < wave.number[INVADER_MINER]  ; ++i) {
            Invader *m = wave.miners[i];
            if (m->x == INVADER_KILLED) continue;

            m->y += ((irandom(0,3) - 1) * 2);
            if (m->y < 10) m->y = 10;
            if (m->y > 50) m->y = 50;

            // Move the Miner in the x-axis and handle
            // x-axis screen wraps
            int16_t dx = player.x - m->x;
            if (abs(dx) > (PLAY_FIELD_WIDTH >> 1)) dx *= -1;

            // Add a wobble to the x-axis speed
            if (dx < 0) {
                dx = -2 - irandom(0,2);
            } else if (dx > 0) {
                dx = 2 + irandom(0,2);
            }

            m->x = check_x(m->x + dx);

            // Will it lay a mine?
            // If it's not too close to the last one...
            if ((irandom(0,100) > 60) && (abs(m->x - m->last_mine_x) > 16)) {
                new_mine(m->x, m->y);
                m->last_mine_x = m->x;
            }
        }
    }
}


/*
 * BEAMERS
 */
void init_beamers() {
    // Clear any existing Beamers and set the initial total to zero
    if (game.level > 1) {
        free_invaders(INVADER_BEAMER);

        // Determine the number of Beamers in the wave
        uint8_t n = 2 + ((game.level >> 1) - 1);
        if (n < 2) n = 2;
        if (n > MAX_BEAMERS) n = MAX_BEAMERS;
        wave.number[INVADER_BEAMER] = n;

        // Roll the Beamers
        for (uint8_t i = 0 ; i < n ; ++i) {
            Invader *b = alloc(sizeof(Invader));
            wave.beamers[i] = b;
            b->type = INVADER_BEAMER;
            b->x = irandom(10, 622);
            b->y = irandom(20, 40);
        }
    }
}


void move_beamers() {
    if (wave.number[INVADER_BEAMER] > 0){
        for (uint8_t i = 0 ; i < wave.number[INVADER_BEAMER] ; ++i) {
            Invader *b = wave.beamers[i];
            if (b->x == INVADER_KILLED) continue;

            // Get distance to player
            int16_t dx = player.x - b->x;
            int8_t  dy = player.y - b->y;

            // Handle the screen wrap
            if (abs(dx) > (PLAY_FIELD_WIDTH >> 1)) dx *= -1;

            // Convert the x-axis distance
            if (dx < 0) {
                dx = -2;
            } else if (dx > 0) {
                dx = 2;
            }

            // Move the Beamer in the x-axis and handle x-axis screen wraps
            b->x = check_x(b->x + dx);

            // Move the Beamer in the y-axis
            if (dy < 0) {
                dy = -2;
            } else if (dy > 0) {
                dy = 2;
            }

            // Step out of a narrow fire zone
            if (dy == 0) {
                dy = 2 * (irandom(0,3) - 1);
            } else {
                dy += (irandom(0,3) - 1);
            }

            b->y = check_y(b->y + dy);
            b->direction = (dx <= 0) ? LEFT : RIGHT;
        }
    }
}


/*
 * KILLERS
 */
void init_killers() {
    // Clear any existing Killers and set the initial total to zero
    free_invaders(INVADER_KILLER);
}


void new_killer(int16_t x) {
    // A Grabber has converted, so create a new Killer
    // There can only be as many Killers as there were Grabbers
    wave.number[INVADER_KILLER] = count_invader(INVADER_KILLER);
    if (wave.number[INVADER_KILLER] == MAX_GRABBERS) return;

    // Roll one killer, either in the first empty slot, or
    // the first filled-but-killed slot
    int8_t idx = new_invader(INVADER_KILLER);
    if (idx != INVADER_KILLED) {
        Invader *k = wave.killers[idx];
        k->type = INVADER_KILLER;
        k->x = check_x(x);
        k->y = 1;
        k->dy = 1;
    }
}


void move_killers() {
    // Move any Killers in the game
    if (wave.number[INVADER_KILLER] > 0) {
        for (uint8_t i = 0 ; i < MAX_GRABBERS ; ++i) {
            if (wave.killers[i] == NULL) continue;

            Invader *k = wave.killers[i];
            if (k->x == INVADER_KILLED) continue;

            // Get distance to player
            int16_t dx = player.x - k->x;
            int16_t dy = player.y - k->y;

            // Handle the screen wrap
            if (abs(dx) > (PLAY_FIELD_WIDTH >> 1)) dx *= -1;

            // Convert the x-axis distance
            if (dx < 0) {
                dx = -1;
            } else if (dx > 0) {
                dx = 1;
            }

            // Move the Killer in the x-axis
            k->x = check_x(k->x + dx);

            // Move the Killer in the y-axis: fast near the player,
            // bobbing along the top otherwise
            if (abs(dx) <= 20) {
                // Dive-bomb the player
                k->dy = ((dy & 0x8000) > 0) ? -4 : 4;
                k->y += k->dy;

                if (k->y > 55) {
                    k->y = 54;
                    k->dy = -3;
                }
            } else {
                k->y += k->dy;

                if (k->y < 0) {
                    k->y = 1;
                    k->dy = 1 + irandom(1,1);
                }

                if (k->y > 10) {
                    k->y = 9;
                    k->dy = -1 - irandom(1,1);
                }

                // Will the Killer fire?
                if (irandom(0,100) > (64 - game.level)) {
                    fire_bullet(k->x, k->y);
                }
            }
        }
    }
}


/*
 * MINES
 */
void init_mines() {
    // Clear any existing Mines and set the initial total to zero
    free_invaders(INVADER_MINE);
}


void new_mine(int16_t x, int8_t y) {
    // A Miner has planted a mine
    // First update the number
    wave.number[INVADER_MINE] = count_invader(INVADER_MINE);
    if (wave.number[INVADER_MINE] == MAX_MINES) return;

    // Roll one Mine, either in the first empty slot, or
    // the first filled-but-killed slot
    int8_t idx = new_invader(INVADER_MINE);
    if (idx != INVADER_KILLED) {
        Invader *k = wave.mines[idx];
        k->type = INVADER_MINE;
        k->x = check_x(x);
        k->y = check_y(y + 8);;
        k->dy = 0;
    }
}


/*
 * BERSERKERS
 */
void init_berserkers() {
    // Clear any existing Berserkers and set the initial total to zero
    free_invaders(INVADER_BERSERKER);
}


void new_berserkers(int16_t x, int8_t y) {
    // Place three Berserkers around the position of the former Beamer
    wave.number[INVADER_BERSERKER] = count_invader(INVADER_BERSERKER);
    if (wave.number[INVADER_BERSERKER] == 3 * wave.number[INVADER_BEAMER]) return;

    int8_t pos[] = {4,-8,-8,9,8,6};
    for (uint8_t i = 0 ; i < 3 ; ++i) {
        int8_t idx = new_invader(INVADER_BERSERKER);
        if (idx != INVADER_KILLED) {
            Invader *k = wave.berserkers[idx];
            k->type = INVADER_BERSERKER;
            k->x = check_x(x + pos[i * 2]);
            k->y = check_y(y + pos[i * 2 + 1]);
            k->dy = 0;
            ++wave.number[INVADER_BERSERKER];
        }
    }
}


void move_berserkers() {
    if (wave.number[INVADER_BERSERKER] > 0) {
        for (uint8_t i = 0 ; i < MAX_BERSERKERS ; ++i) {
            if (wave.berserkers[i] == NULL) continue;

            Invader *b = wave.berserkers[i];
            if (b->x == INVADER_KILLED) continue;

            // Get distance to player
            int16_t dist_x = player.x - b->x;
            int8_t  dist_y = player.y - b->y;
            int16_t dx = 0;
            int8_t  dy = 0;

            // Handle the screen wrap
            uint16_t abs_dx = (dist_x < 0) ? (dist_x * -1) : dist_x;
            if (abs_dx > 320) dist_x *= -1;

            // Convert the x-axis distance
            if (dist_x < 0) dx = -2;
            if (dist_x > 0) dx = 2;

            // Move the Beamer in the y-axis
            if (dist_y < 0) dy = -2;
            if (dist_y > 0) dy = 2;

            // Step out of a narrow fire zone
            if (dy == 0) {
                dy = 3 * (irandom(0,3) - 1);
            } else {
                dy += (3 * (irandom(0,3) - 1));
            }

            dx += (2 * (irandom(0,3) - 1));

            // Move the Berserker
            b->x = check_x(b->x + dx);
            b->y = check_y(b->y + dy);

            // Will the Berserker fire?
            if (irandom(0, 100) > (50 - game.level)) {
                fire_bullet(b->x, b->y);
            }
        }
    }
}


/*
 * CHASERS
 */
void init_chasers() {
    // Clear any existing Chasers and set the initial total to zero
    free_invaders(INVADER_CHASER);
}

void new_chaser() {
    // First update the number
    wave.number[INVADER_CHASER] = count_invader(INVADER_CHASER);
    if (wave.number[INVADER_CHASER] == MAX_CHASERS) return;

    // Roll one killer, either in the first empty slot, or
    // the first filled-but-killed slot
    int8_t idx = new_invader(INVADER_CHASER);
    if (idx != INVADER_KILLED) {
        Invader *k = wave.chasers[idx];
        k->type = INVADER_CHASER;
        k->x = irandom(10, PLAY_FIELD_WIDTH - 20);
        k->y = 40;
        k->dy = 0;
    }
}
