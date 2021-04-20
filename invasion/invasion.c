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


uint16_t scores[8] = {500,150,250,250,1000,150,150,250};


/*
 *  Initialisation Functions
 */
void setup() {
    // Set up the game hardware

    // Set up I2C
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    // Set up the OLED
    ssd1306_init(128, 64);

    // Set up the speaker
    gpio_init(PIN_SPEAKER);
    gpio_set_dir(PIN_SPEAKER, GPIO_OUT);
    gpio_put(PIN_SPEAKER, false);

    // Set up the Fire button
    gpio_init(PIN_FIRE_BUTTON);
    gpio_set_dir(PIN_FIRE_BUTTON, GPIO_IN);
    gpio_pull_down(PIN_FIRE_BUTTON);

    // Set up the Teleport button
    gpio_init(PIN_BOMB_BUTTON);
    gpio_set_dir(PIN_BOMB_BUTTON, GPIO_IN);
    gpio_pull_down(PIN_BOMB_BUTTON);

    // Set up the X- and Y-axis joystick input
    adc_init();
    adc_gpio_init(PIN_X);
    adc_gpio_init(PIN_Y);

    // Use one of the Pico's other analog inputs
    // to seed the random number generator
    adc_gpio_init(28);
    adc_select_input(2);
    srand(adc_read());

    // Randomise using TinyMT
    // https://github.com/MersenneTwister-Lab/TinyMT
    tinymt32_init(&tinymt_store, adc_read());

    // Set the address of the buffer we'll draw to
    draw_buffer = &oled_buffer[0];

    // Clear all the timers...
    for (uint8_t i = 0 ; i < NUMBER_OF_TIMERS ; ++i) {
        timers[i] = 0;
    }
}


void init_game() {
    // Create a new game
    // Set up the game-wide variables
    game.bomb_count = 3;
    game.lives = 2;
    game.level = 1;
    game.in_play = true;
    game.is_firing = false;
    game.zap_state = true;
    game.next_bonus = 1;
    game.score = 0;
    game.high_score = 0;

    // Set up level data that needs to be set
    // BEFORE a new level is set up (because
    // it checks these for non-zero values)
    for (uint8_t i = 0 ; i < INVADER_TYPE_COUNT + 1 ; ++i) {
        wave.number[i] = 0;
    }

    // Set up the wave's array of invader arrays
    wave.invaders[INVADER_GRABBER]   = wave.grabbers;
    wave.invaders[INVADER_CHASER]    = wave.chasers;
    wave.invaders[INVADER_KILLER]    = wave.killers;
    wave.invaders[INVADER_BEAMER]    = wave.beamers;
    wave.invaders[INVADER_BERSERKER] = wave.berserkers;
    wave.invaders[INVADER_MINER]     = wave.miners;
    wave.invaders[INVADER_MINE]      = wave.mines;
}


void init_planet() {
    // Set up a new play field
    // This is called at the start of the game and
    // again after every four completed waves

    // Create the mountain backdrop
    // Values here will appear every two pixels
    uint8_t start = irandom(20, 41);    // 20-60
    wave.backdrop[0] = start;
    int8_t delta = 1;
    uint8_t min = 20;
    uint8_t max = 60;
    uint16_t min_start = (PLAY_FIELD_WIDTH >> 1) - (start - 20);
    uint16_t max_start = (PLAY_FIELD_WIDTH >> 1) - (60 - start);

    for (uint16_t i = 1 ; i < (PLAY_FIELD_WIDTH >> 1) ; ++i) {
        start += delta;

        // Do we need to zero in on the start?
        if (i > min_start) ++min;
        if (i > max_start) --max;

        // Check for a bounce at the end of the range, or
        // allow for a random switch
        if (start < min) {
            start = min + 1;
            delta *= -1;
        } else if (start > max) {
            start = max - 1;
            delta *= -1;
        } else if (irandom(0, 100) > 80) {
            delta *= -1;
        }

        wave.backdrop[i] = start;
    }

    // Create a new set of crystals, clearing out the old
    // set if there's one present
    if (wave.number[CRYSTAL_INDEX] > 0) {
        for (uint8_t i = 0 ; i < MAX_CRYSTALS ; ++i) {
            if (wave.crystals[i] != NULL) {
                free(wave.crystals[i]);
                wave.crystals[i] = NULL;
            }
        }
    }

    // Place the Caloxin Crystals in a set of 32 bands dividing
    // the play field so that they are not too close together
    wave.number[CRYSTAL_INDEX] = 10;
    if (game.level % 4 == 0) wave.number[CRYSTAL_INDEX] += (game.level >> 2);
    if (wave.number[CRYSTAL_INDEX] > 30) wave.number[CRYSTAL_INDEX] = 30;
    for (uint8_t i = 0 ; i < wave.number[CRYSTAL_INDEX] ; ++i) {
        Crystal *c = alloc(sizeof(Crystal));
        wave.crystals[i] = c;

        bool placed = false;
        do {
            // Randomly pick a band
            c->x = irandom(1, CRYSTAL_BAND_WIDTH - 1);  // Range 1-18
            placed = true;

            // Check there's not a crystal in the band already
            for (uint8_t j = 0 ; i < i ; ++i) {
                Crystal *a = wave.crystals[j];
                if (a->x == c->x) {
                    placed = false;
                    break;
                }
            }
        } while (!placed);
    }

    // Convert the band values into x coordinates and add the y coordinate
    for (uint8_t i = 0 ; i < wave.number[CRYSTAL_INDEX] ; ++i) {
        Crystal *c = wave.crystals[i];
        c->x = c->x * 32 - 16;
        c->y = GROUND_LEVEL;
        c->drop_height = 0;
        c->held = false;
    }

    // Place a wave of invaders
    init_wave();

    // Place the player in the middle of the field
    player.x = (PLAY_FIELD_WIDTH >> 1);
    player.y = 20;
    player.direction = irandom(0, 1);
    player.carrying = NO_CRYSTAL;

    // Clear all the timers
    for (uint8_t i = 0 ; i < NUMBER_OF_TIMERS ; ++i) {
        timers[i] = 0;
    }

    wave.is_in_space = false;
}


void init_wave() {
    // Place a new wave of invaders
    // TODO Assign types by wave number
    init_grabbers(wave.number[CRYSTAL_INDEX]);
    init_killers();
    init_miners();
    init_mines();
    init_beamers();
    init_berserkers();
    //init_chasers();

    // Clear the bullet list
    for (uint8_t i = 0 ; i < MAX_BULLETS * 4 ; ++i) {
        bullets[i] = NO_CRYSTAL;
    }
}


/*
 * Game Loop Management Functions
 */
void game_loop() {
    // Loop while we're in play
    // Death will cause us to break out to the main
    // game-to-game loop
    int8_t delta = 2;
    uint16_t x = 2048;
    uint16_t y = 2048;
    uint8_t dir = 0;
    int16_t nx = player.x;
    int8_t  ny = player.y;

    do {
        // This block primarily checks the controls
        // and triggers actions accordingly

        // Read joystick analog output
        adc_select_input(0);
        x = adc_read();
        adc_select_input(1);
        y = adc_read();

        // Joystick is pointing in a direction, so
        // get the direction the player has chosen
        // 0 = up
        // 2 = down
        // 1 = right
        // 3 = left
        dir = get_direction(x, y);
        nx = player.x;
        ny = player.y;

        if ((dir & MOVE_UP) > 0)   --ny;
        if ((dir & MOVE_DOWN) > 0) ++ny;

        delta = player.direction == 0 ? -2 : 2;

        if ((dir & FAST_RIGHT) > 0) {
            delta = 4;
            player.direction = RIGHT;
        } else if ((dir & SLOW_RIGHT) > 0) {
            player.direction = RIGHT;
        }

        if ((dir & FAST_LEFT) > 0) {
            delta = -4;
            player.direction = LEFT;
        } else if ((dir & SLOW_RIGHT) > 0) {
            player.direction = LEFT;
        }

        nx += delta;

        // Handle the x-axis screen wrap
        if (nx < 0) nx += (PLAY_FIELD_WIDTH - 1);
        if (nx >= PLAY_FIELD_WIDTH) nx -= (PLAY_FIELD_WIDTH - 1);

        // Handle the y-axis limits
        if (ny < 0) ny = 0;
        if (ny > 60) ny = 60;

        // Set the new square for rendering later
        player.x = nx;
        player.y = ny;

        // TODO
        // Check for collision at new location

        // Get the current time for debounce checks
        uint32_t now = time_us_32();

        if (gpio_get(PIN_FIRE_BUTTON)) {
            // Button pressed: debounce it
            if (timers[TIMER_FIRE_DEBOUNCE] == 0) {
                // Set debounce timer
                timers[TIMER_FIRE_DEBOUNCE] = now;
            } else if (now - timers[TIMER_FIRE_DEBOUNCE] > DEBOUNCE_TIME_US) {
                // Prime the laser if it's unprimed, ie. show the crosshair
                game.is_firing = true;
                game.zap_state = !game.zap_state;
            }
        } else {
            game.is_firing = false;
            timers[TIMER_FIRE_DEBOUNCE] = 0;
        }

        if (gpio_get(PIN_BOMB_BUTTON)) {
            // Player can only teleport if they have walked over the
            // teleport square and they are not firing the laser
            if (game.bomb_count > 0) {
                if (timers[TIMER_BOMB_DEBOUNCE] == 0) {
                    // Set debounce timer
                    timers[TIMER_BOMB_DEBOUNCE] = now;
                } else if (now - timers[TIMER_BOMB_DEBOUNCE] > DEBOUNCE_TIME_US) {
                    // Teleport
                    timers[TIMER_BOMB_DEBOUNCE] == 0;
                    explode_the_bomb();
                }
            }
        }

        // Draw the world and handle
        update_world();

    // At the end of the loop check to see if the player is
    // still alive -- and drop out if they are not
    } while (game.in_play);

    // Show the death view when the game loop exits
    death();

    // At this point, the code returns to the outer loop
    // in 'main()' to initialize a new game and start it
}


uint8_t get_direction(uint16_t x, uint16_t y) {
    // Get player movement action from the analog input
    // Favour movement over rotation
    uint8_t value = 0;
    if (y > UPPER_LIMIT) value |= MOVE_UP;
    if (y < LOWER_LIMIT) value |= MOVE_DOWN;

    if (x > FU_LIMIT) {
        value |= FAST_LEFT;
    } else if (x > UPPER_LIMIT) {
        value |= SLOW_LEFT;
    }

    if (x < FL_LIMIT) {
        value |= FAST_RIGHT;
    } else if (x < LOWER_LIMIT) {
        value |= SLOW_RIGHT;
    }

    return value;
}


void update_world() {
    // Render the graphics as required -- currently on a timer
    // and process the movement of invaders, bullets etc.
    uint32_t now = time_us_32();

    // Render the screen
    if (now - timers[TIMER_SCREEN_DRAW] > ANIM_TIME_US) {
        ssd1306_clear();

        // Draw the score
        char score_string[6] = "000000";
        sprintf(score_string, "%06d", game.score);
        ssd1306_text(0, 0, score_string, false, false);

        // Draw the wave number
        sprintf(score_string, "%d", game.level);
        ssd1306_text((128 - 5 * strlen(score_string) >> 1), 0, score_string, false, false);

        // Draw in remaining lives
        for (uint8_t i = 0 ; i < game.lives ; ++i) {
            draw_ship(110, i * 4, RIGHT, false, false);
        }

        // Draw in remaining smart bombs
        for (uint8_t i = 0 ; i < game.bomb_count ; ++i) {
            draw_bomb(120, i * 4);
        }

        // Does the view include the play field wrap?
        bool split = is_screen_split();

        // What's the play field co-ordinate of the
        // screen's left-most column?
        int16_t start = get_start();

        // Draw in the backdrop
        int16_t back_index = (start >> 1);
        for (uint16_t i = 0 ; i < 128 ; i += 2) {
            uint8_t index = wave.backdrop[back_index];
            ssd1306_plot(i, index, 1);
            ++back_index;
            if (back_index > 319) back_index = 0;
        }

        // Draw in the crystals
        for (uint8_t i = 0 ; i < wave.number[CRYSTAL_INDEX] ; ++i) {
            Crystal *c = wave.crystals[i];
            // If the crystal has been removed or is being carried, skip it
            if (c->x == CRYSTAL_TAKEN || c->held) continue;

            // Check if the crystal is on screen; plot it if it is
            if (on_screen(c->x)) {
                uint8_t x = get_coord(c->x);
                ssd1306_line(x, c->y, x, c->y + 3, 1, 1);
                ssd1306_line(x + 1, c->y, x + 1, c->y + 3, 1, 1);
            }
        }

        // Draw in the invaders
        for (uint8_t i = 0 ; i < INVADER_TYPE_COUNT ; ++i) {
            if (wave.number[i] > 0) {
                for (uint8_t j = 0 ; j < wave.number[i] ; ++j) {
                    if (wave.invaders[i][j] == NULL) continue;
                    Invader *k = wave.invaders[i][j];
                    if (k->x != INVADER_KILLED && on_screen(k->x)) {
                        draw_monster(k);
                    }
                }
            }
        }

        // Draw in the bullets
        for (uint8_t i = 0 ; i < MAX_BULLETS * 4 ; i += 4) {
            if ((bullets[i] != NO_CRYSTAL) && (on_screen(bullets[i]))) {
                ssd1306_plot(get_coord(bullets[i]), bullets[i + 1], 1);
            }
        }

        // Draw in the ship
        draw_ship(60, player.y, player.direction, (game.is_firing && game.zap_state), (player.carrying != NO_CRYSTAL));

        // Write out the display buffer
        ssd1306_draw();

        // Set the latest render time
        timers[TIMER_SCREEN_DRAW] = now;
    }

    // Did the player put down a crystal?
    if (player.carrying != NO_CRYSTAL && player.y > 58) place_crystal();

    // Did the player catch a falling crystal?
    if (player.carrying == NO_CRYSTAL) catch_crystal();

    // Is the player firing the laser? If so, did it hit anything?
    if (game.is_firing) fire_laser();

    // Check for bonuses
    check_bonus();

    // Move the monsters
    move_monsters();
    if (live_monster_count() == 0) {
        // All the monsters are dead
        wave_cleared();
    }

    // Move the crystals
    // TODO Make this happen every second
    move_crystals();
    if (live_crystal_count() == 0) {
        // TODO Space fight
        wave.is_in_space = true;
    }

    // Move any bullets
    move_bullets();
}


/*
 * Player Actions Functions
 */
void catch_crystal() {
    // Check crystals to see if the player has caught
    // one the that has been dropped
    for (uint8_t i = 0 ; i < wave.number[CRYSTAL_INDEX] ; ++i) {
        Crystal *c = wave.crystals[i];
        if ((c->x == CRYSTAL_TAKEN) || (c->drop_height < 5)) continue;
        if ((c->y - player.y >= 3) && (c->y - player.y <= 7)) {
            if ((c->x > player.x) && (c->x < player.x + 8)) {
                c->held = true;
                player.carrying = i;
                return;
            }
        }
    }
}


void place_crystal() {
    // The player successfully replaced a crystal on the ground
    Crystal *c = wave.crystals[player.carrying];
    c->x = player.x;
    c->y = GROUND_LEVEL;
    c->held = false;
    c->drop_height = 0;

    // Update player stats
    game.score += scores[SCORE_RECOVERED_CRYSTAL];
    player.carrying = NO_CRYSTAL;
}


void fire_laser() {
    // See if the firing laser hit anything
    bool hit = did_hit_invader(false);

    // Only render explosions if they occurred
    if (hit) ssd1306_draw();
}


void explode_the_bomb() {
    // Detonate a smart bomb
    // NOTE We've already checked there was a bomb to explode
    --game.bomb_count;

    // Flash the screen
    bool state = true;
    for (uint8_t i = 0 ; i < 4 ; ++i) {
        ssd1306_inverse(state);

        if (state) {
            for (uint16_t i = 200 ; i < 1001 ; i += 50) tone(i, 5, 0);
        } else {
            for (uint16_t i = 1000 ; i > 199 ; i -= 50) tone(i, 5, 0);
        }

        state = !state;
        //sleep_ms(50);
    }

    // Find which invaders, if any, were killed
    did_hit_invader(true);

    // Zap bullets too
    for (uint8_t i = 0 ; i < MAX_BULLETS * 4 ; i += 4) {
        if (bullets[i] == NO_CRYSTAL) continue;
        if (on_screen(bullets[i])) {
            bullets[i] = NO_CRYSTAL;
        }
    }
}


bool did_hit_invader(bool smart_bomb) {
    // Check all the invader types to see if any have been hit by
    // a laser burst or a smart bomb explosion
    bool did_hit = false;
    for (uint8_t i = 0 ; i < INVADER_TYPE_COUNT ; ++i) {
        if (wave.number[i] > 0) {
            uint8_t inv_max = get_max_invaders(i);
            for (uint8_t j = 0 ; j < inv_max ; ++j) {
                if (wave.invaders[i][j] == NULL) continue;

                Invader *k = wave.invaders[i][j];
                if ((k->x == INVADER_KILLED) || !on_screen(k->x)) continue;

                // Did we have a hit?
                bool hit = false;
                if (!smart_bomb) {
                    hit = did_hit_with_laser(k->x, k->y);
                    if (hit) {
                        tone(1000,20,0);
                        draw_boom(get_coord(k->x), k->y);
                    }
                }

                if (smart_bomb || hit) {
                    if (i == INVADER_GRABBER) {
                        // Drop any crystals being carried
                        if (k->crystal != NO_CRYSTAL) {
                            Crystal *c = wave.crystals[k->crystal];
                            c->held = false;
                            c->drop_height = GROUND_LEVEL - c->y;
                            k->crystal = NO_CRYSTAL;
                        }
                    }

                    if (i == INVADER_BEAMER) {
                        // Spawn three berserkers
                        new_berserkers(k->x, k->y);
                    }

                    // Mark the invader as killed and add the score
                    k->x = INVADER_KILLED;
                    free(wave.invaders[i][j]);
                    wave.invaders[i][j] = NULL;
                    game.score += scores[i];
                    did_hit = true;
                }
            }
        }
    }

    return did_hit;
}


bool did_hit_with_laser(uint16_t x, uint8_t y) {
    // Did we have a hit?
    if ((player.y + 2 >= y) && (player.y + 2 <= y + 8)) {
        if (player.direction == LEFT) {
            if ((player.x - x <= 18) && (player.x - x > 0)) return true;
        } else {
            if ((x - player.x <= 18) && (x - player.x > 0)) return true;
        }
    }

    return false;
}


/*
 * Game Outcome Functions
 */
void wave_cleared() {
    // The player has cleared a wave of invaders
    // Invert the screen
    ssd1306_inverse(true);

    for (uint16_t i = 500 ; i < 2001 ; i += 50) {
        tone(i, 20, 0);
    }

    // Add up the wave-clearance points and increase the level
    game.score += (100 * (game.lives + 1) * game.level);
    check_bonus();
    ++game.level;

    // Set up the next level every fourth wave, otherwise
    // just round up some new invaders.
    // NOTE 'init_planet()' calls 'init_wave()'
    if (game.level % 4 == 0) {
        init_planet();
    } else {
        init_wave();
    }

    // Pause for breath and de-invert the screen
    sleep_ms(1500);
    ssd1306_inverse(false);
}


void check_bonus() {
    // Check for bonuses -- ie. x0,000 points exceeded
    if (game.score >= game.next_bonus * 10000) {
        // Extra life and smart bomb every 10000 points
        ++game.lives;
        ++game.bomb_count;
        ++game.next_bonus;

        // Enforce maxima
        if (game.lives > 3) game.lives = 3;
        if (game.bomb_count > 5) game.bomb_count = 5;
    }
}


void death() {
    // TODO
}


/*
 * Utilities
 */
bool on_screen(int16_t x) {
    // Returns whether the specified coordinate is currently on screen
    // TODO Check against entity's width
    bool split = is_screen_split();
    int16_t start = get_start();

    if (split) {
        if (((x >= start) && (x < PLAY_FIELD_WIDTH)) || ((x >= 0) && (x < 128 - PLAY_FIELD_WIDTH + start))) {
            return true;
        }
    } else {
        if ((x >= start) && (x < start + 128)) {
            return true;
        }
    }

    return false;
}


int8_t get_coord(int16_t x) {
    // Returns the screen coordinate mapped by a play field coordinate
    // NOTE x only as y has a 1:1 mapping in both frames of reference
    bool split = is_screen_split();
    int16_t start = get_start();
    if (split && x < 128) return (int8_t)(x + (PLAY_FIELD_WIDTH - start));
    return (int8_t)(x - start);
}


int16_t get_start() {
    // Return the play field x-axis co-ordinate of the left-most display column
    int16_t start = player.x - SCREEN_HALF_WIDTH;
    if (start < 0) start = PLAY_FIELD_WIDTH + start;
    return start;
}


bool is_screen_split() {
    // Is the screen wrap visible on screen?
    return ((player.x < SCREEN_HALF_WIDTH - 1) || (player.x >= PLAY_FIELD_WIDTH - SCREEN_HALF_WIDTH));
}


void play_intro() {
    // Get the text lengths
    uint16_t l1 = 128 - ssd1306_text_length("PLANET INVASION 1.0");
    uint16_t l2 = 128 - ssd1306_text_length("BY TONY SMITH");
    uint16_t l3 = 128 - ssd1306_text_length("& STEVE GEISEKING");

    // Draw a Spectral Associates-style screen:
    // First the box...
    ssd1306_clear();
    ssd1306_text(1,0,"*********************", false, false);
    for (uint8_t i = 8 ; i < 35 ; i += 8) {
        ssd1306_text(1,i,"*", false, false);
        ssd1306_text(121,i,"*", false, false);
    }
    ssd1306_text(1,32,"*********************", false, false);
    ssd1306_rect(0,40,128,24,1,true);

    // ...then the text
    ssd1306_text((l1 >> 1), 8,  "PLANET INVASION 1.0", false, false);
    ssd1306_text((l2 >> 1), 16, "BY TONY SMITH", false, false);
    ssd1306_text((l3 >> 1), 24, "& STEVE GIESEKING", false, false);
    ssd1306_draw();
    ssd1306_inverse(true);

    // Wait for a keypress
    inkey();

    // Clear the screen and de-inverse
    ssd1306_clear();
    ssd1306_draw();
    ssd1306_inverse(false);
}


/*
 * Runtime start
 *
 */
int main() {

    // Setup the hardware
    setup();

    // Start a new game
    play_intro();

    // Play the game
    while (1) {
        // Set up the environment, once per game
        init_game();
        init_planet();

        // ...and start play
        game_loop();
    }

    return 0;
}
