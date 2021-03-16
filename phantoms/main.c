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
    gpio_init(PIN_TELE_BUTTON);
    gpio_set_dir(PIN_TELE_BUTTON, GPIO_IN);
    gpio_pull_down(PIN_TELE_BUTTON);

    // Set up the X- and Y-axis joystick input
    adc_init();
    adc_gpio_init(PIN_X);
    adc_gpio_init(PIN_Y);

    // Use one of the Pico's other analog inputs
    // to seed the random number generator
    adc_gpio_init(28);
    adc_select_input(2);
    srand(adc_read());

    // Set up the LED to flash with the speaker
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    gpio_put(PIN_LED, false);

    // Make the graphic frame rects
    // NOTE These are pixel values
    uint8_t coords[] = {0,0,128,64,
                        11,5,106,54,
                        24,10,80,44,
                        36,15,56,34,
                        47,20,34,24,
                        55,25,18,14,
                        61,27,6,10};

    for (uint8_t i = 0 ; i < sizeof(coords) ; i += 4) {
        Rect a_rect;
        a_rect.origin_x = coords[i];
        a_rect.origin_y = coords[i + 1];
        a_rect.width = coords[i + 2];
        a_rect.height = coords[i + 3];
        rects[i >> 2] = a_rect;
    }
}


void init_game() {
    // Reset the main game control structure
    game.in_play = false;
    game.show_reticule = false;
    game.is_firing = false;
    game.can_teleport = false;
    game.is_joystick_centred = true;
    game.level_score = 0;
    game.audio_range = 4;
    game.phantoms = 0;
    game.level = 1;
    game.zap_time = 0;
    game.debounce_count_press = 0;
    game.debounce_count_release = 0;
    game.tele_x = ERROR_CONDITION;
    game.tele_y = ERROR_CONDITION;

    chase_mode = false;
}


void init_phantoms() {
    // Reset the phantoms structure
    for (uint8_t i = 0 ; i < 3 ; ++i) {
        Phantom* p = &phantoms[i];
        p->x = ERROR_CONDITION;
        p->y = ERROR_CONDITION;
        p->hp = 1;
        p->direction = 0;
        p->rev = false;
    }
}


void create_world() {
    // Generate and populate a new maze which happens
    // at the start of a new game and at the start of
    // each level. A level jump is triggered when all the
    // current phantoms have beebn dispatched

    if (game.in_play) {
        game.level++;
        game.phantoms = 0;
    } else {
        // Reset the game
        if (game.level > 0) init_game();
        game.in_play = true;
    }

    // Initialise the current map
    map_init();
    last_draw = 0;

    // Generate phantoms
    game.phantoms = game.level;
    if (game.phantoms > 3) game.phantoms = 3;

    // Reset the the phant
    init_phantoms();

    // Add the phantoms to the map, everywhere but empty
    uint8_t empty_quad = irandom(0, 4);
    uint8_t phantom_quad = irandom(0, 4);
    for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
        Phantom* p = &phantoms[i];
        while (true) {
            // Pick a random co-ordinate
            uint8_t x = irandom(0, 6);
            uint8_t y = irandom(0, 6);

            // Make sure the phantom's not in the fourth quadrant
            if (phantom_quad == empty_quad) ++phantom_quad;
            if (phantom_quad > 3) phantom_quad = 0;

            // Adjust (x,y) to outer part of quadrant
            if (phantom_quad == 1) {
                x = 19 - x;
            } else if (phantom_quad == 2) {
                x = 19 - x;
                y = 19 - y;
            } else if (phantom_quad == 3) {
                y = 19 - y;
            }

            // If the chosen square is valid, use it
            if (get_square_contents(x, y) == MAP_TILE_CLEAR) {
                p->x = x;
                p->y = y;
                break;
            }

            // Move to the next quad
            ++phantom_quad;
        }
    }

    // Place the player near the centre
    uint8_t x = 9;
    uint8_t y = 9;

    while (true) {
        x = 9 + (irandom(0, 3) - 1);
        y = 9 + (irandom(0, 3) - 1);
        if (get_square_contents(x, y) == MAP_TILE_CLEAR) break;
    }

    player_x = x;
    player_y = y;
    player_direction = irandom(0, 4);
}


void game_loop() {
    // Read the current joystick position.
    // If it's not in the deadzone, then determine
    // which direction it's in (up, down, left or right).
    // If it's in the deadzone, check if the player is trying
    // to fire an arrow.

    while (game.in_play) {
        bool is_dead = false;

        // Read joystick analog output
        adc_select_input(0);
        uint16_t x = adc_read();
        adc_select_input(1);
        uint16_t y = adc_read();

        // Can't move with reticule showing
        if (!game.show_reticule && !game.is_firing && check_joystick(x,y)) {
            // Joystick is pointing in a direction, so
            // get the direction the player has chosen
            // 0 = forward
            // 2 = backward
            // 1 = rotate right
            // 3 = rotate left
            uint8_t dir = get_direction(x, y);
            uint8_t nx = player_x;
            uint8_t ny = player_y;

            if (dir == MOVE_FORWARD || dir == MOVE_BACKWARD) {
                // Move player forward or backward if we can
                if (player_direction == DIRECTION_NORTH) ny += (dir == MOVE_FORWARD ? -1 : 1);
                if (player_direction == DIRECTION_SOUTH) ny += (dir == MOVE_FORWARD ? 1 : -1);
                if (player_direction == DIRECTION_EAST) nx += (dir == MOVE_FORWARD ? 1 : -1);
                if (player_direction == DIRECTION_WEST) nx += (dir == MOVE_FORWARD ? -1 : 1);

                if (ny < 20 && nx < 20 && !check_hazard(nx, ny)) {
                    player_x = nx;
                    player_y = ny;
                }
            } else if (dir == TURN_RIGHT) {
                // Turn player right
                ++player_direction;
                if (player_direction > DIRECTION_WEST) player_direction = DIRECTION_NORTH;
            } else if (dir == TURN_LEFT) {
                // Turn player left
                --player_direction;
                if (player_direction > DIRECTION_WEST) player_direction = DIRECTION_WEST;
            }
        }

        // Check for button presses
        uint32_t now = time_us_32();

        if (gpio_get(PIN_FIRE_BUTTON)) {
            if (game.can_fire) {
                // Button pressed: debounce it
                if (game.debounce_count_press == 0) {
                    // Set debounce timer
                    game.debounce_count_press = now;
                } else if (now - game.debounce_count_press > DEBOUNCE_TIME_US) {
                    // Prime the laser if it's unprimed
                    if (!game.show_reticule) {
                        game.show_reticule = true;
                        game.debounce_count_press = 0;
                    }
                }
            }
        } else {
            // Button released: check it was previously
            // pressed down, ie. 'game.show_reticule' is true
            if (game.show_reticule) {
                if (game.debounce_count_release == 0) {
                    // Set debounce timer
                    game.debounce_count_release = now;
                } else if (now - game.debounce_count_release > DEBOUNCE_TIME_US) {
                    // Fire the laser
                    game.debounce_count_release == 0;
                    game.show_reticule = false;
                    game.is_firing = true;
                    game.can_fire = false;
                }
            }
        }

        if (gpio_get(PIN_TELE_BUTTON)) {
            // Player can only teleport if they have walked over the teleport square
            // and they are not firing the laser
            if (!game.show_reticule && game.can_teleport) {
                if (game.debounce_count_press == 0) {
                    // Set debounce timer
                    game.debounce_count_press = now;
                } else if (now - game.debounce_count_press > DEBOUNCE_TIME_US) {
                    // Teleport
                    game.debounce_count_press == 0;
                    do_teleport();
                }
            }
        }

        if (!is_dead) {
            // Manage and draw the world
            update_world(time_us_32());

            // Check for a laser burst
            if (game.is_firing) {
                game.is_firing = false;
                fire_laser();
            }
        } else {
            // Player killed by some means
            game.in_play = false;
            death();
            break;
        }
    }
}


bool check_joystick(uint16_t x, uint16_t y) {
    // Check to see if the joystick is currently
    // outside of the central deadzone, and that it
    // has returned to the centre before re-reading
    if (x > UPPER_LIMIT || x < LOWER_LIMIT || y > UPPER_LIMIT || y < LOWER_LIMIT) {
        if (game.is_joystick_centred) {
            // We're good to use the reading, but not
            game.is_joystick_centred = false;
            return true;
        } else {
            return false;
        }
    }

    // Joystick is centred
    game.is_joystick_centred = true;
    return false;
}


uint8_t get_direction(uint16_t x, uint16_t y) {
    // Get player movement action from the analog input
    // Favour movement over rotation
    if (y > UPPER_LIMIT) return MOVE_FORWARD;
    if (y < LOWER_LIMIT) return MOVE_BACKWARD;
    if (x > UPPER_LIMIT) return TURN_LEFT;
    if (x < LOWER_LIMIT) return TURN_RIGHT;

    // Just in case
    return ERROR_CONDITION;
}


bool check_hazard(uint8_t x, uint8_t y) {
    // Check the kind of square the player is standing on
    // and action accordingly -- only teleport squares for now
    uint8_t sc = get_square_contents(x, y);

    if (sc == MAP_TILE_TELEPORTER) {
        // Player has walked on the teleport square - record it
        // and clear the map location
        game.tele_x = x;
        game.tele_y = y;
        game.can_teleport = true;
        set_square_contents(x, y, MAP_TILE_CLEAR);
    }

    // Return 'true' on a bad square, false in a good one
    return !(sc == MAP_TILE_CLEAR || sc == MAP_TILE_TELEPORTER);
}


void update_world(uint32_t now) {
    // Update the world at the end of the move cycle
    // Draw the graphics and animate the phantoms
    if (now - last_draw > ANIM_TIME_US) {
        ssd1306_clear();

        if (chase_mode) {
            draw_screen(phantoms[0].x, phantoms[0].y, phantoms[0].direction);
        } else {
            draw_screen(player_x, player_y, player_direction);
        }


        if (game.show_reticule) {
            ssd1306_rect(64, 26, 2, 5, 1, false);
            ssd1306_rect(64, 33, 2, 5, 1, false);
            ssd1306_rect(58, 32, 5, 2, 1, false);
            ssd1306_rect(65, 32, 5, 2, 1, false);
        }

        ssd1306_draw();
        last_draw = now;
    }

    // Move the phantoms periodically
    if (now - last_phantom_move > PHANTOM_MOVE_TIME_US) {
        last_phantom_move = now;
        move_phantoms();
        check_senses();
    }

    // Check for laser recharge
    if (now - game.zap_time > 2100000) {
        game.zap_time = 0;
        game.can_fire = true;
    }
}


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

    uint8_t i = 0;
    while (phantom_spawns > 0) {
        // Generate more phantoms if we need to
        Phantom* p = &phantoms[i];
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

        ++i;
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


/*
 *  Actions
 */
void check_senses() {
    // Scan around the player for nearby phantoms
    int8_t dx = player_x - 5;
    int8_t dy = player_y - 5;

    for (int8_t i = dx ; i < dx + 10 ; ++i) {
        if (i < 0) continue;
        if (i > 19) break;
        for (int8_t j = dy ; j < dy + 10 ; ++j) {
            if (j < 0) continue;
            if (j > 19) break;
            if (locate_phantom(i, j) != ERROR_CONDITION) {
                gpio_put(PIN_LED, true);
                sleep_ms(5);
                gpio_put(PIN_LED, false);

                // Only play one beep, no matter
                // how many nearby phantoms there are
                return;
            }
        }
    }
}


void do_teleport() {
    // Jump back to the teleport sqaure
    // if the player has walked over it
    // NOTE Only one usage per level
    if (game.can_teleport) {
        game.can_teleport = false;

        // Flash the screen
        bool tstate = false;
        for (uint8_t i = 0 ; i < 10 ; i++) {
            ssd1306_inverse(tstate);
            sleep_ms(40);
            tstate = !tstate;
        }

        // Move the player to the stored square
        player_x = game.tele_x;
        player_y = game.tele_y;
    }
}


void fire_laser() {
    // Hit the front-most facing phantom, if there is one

    // PLAY SOUND

    // Animate the zap
    uint8_t radii[] = {20, 16, 10, 4};
    uint8_t temp_buf[oled_buffer_size];
    memcpy(&temp_buf[0], oled_buffer, oled_buffer_size);

    for (uint8_t i = 0 ; i < 4 ; ++i) {
        ssd1306_circle(64, 32, radii[i], 1, true);
        ssd1306_draw();
        sleep_ms(100);
        memcpy(&oled_buffer[0], temp_buf, oled_buffer_size);
    }

    // Draw the bulletless view
    ssd1306_draw();

    uint8_t n = get_facing_phantom(5);
    if (n != ERROR_CONDITION) {
        // A hit! A palpable hit!
        Phantom* p = &phantoms[n];
        p->hp -= 1;
        if (p->hp == 0) {
            // One dead phantom
            p->x = ERROR_CONDITION;
            ++game.level_score;

            // Briefly invert the screen
            ssd1306_inverse(false);
            sleep_ms(200);

            // Draw without the front phantom
            draw_screen(player_x, player_y, player_direction);
            ssd1306_draw();
            ssd1306_inverse(true);

            // PLAY SOUND

            uint8_t count = 0;
            for (uint8_t i = 0 ; i < game.phantoms ; ++i) {
                Phantom* p = &phantoms[i];
                if (p->x == ERROR_CONDITION) {
                    ++count;
                    if (count == game.phantoms) {
                        // Level complete -- add a phantom, up to 3
                        ++game.level;
                        if (game.level < 4) game.phantoms = game.level;
                    }
                }
            }
        }
    }
}


/*
 * Game Outcomes
 */
void death() {
    // The player has died -- show the map and the score
    ssd1306_clear();
    ssd1306_inverse(false);
    ssd1306_text(0, 0, "SCORE", false, false);
    ssd1306_text(98, 0, "HIGH", false, false);
    ssd1306_text(98, 8, "SCORE", false, false);
    // TODO
    // Show the score
    char score_string[] = "000";
    ssd1306_text(0, 8, score_string, false, false);
    ssd1306_text(98, 16, score_string, false, false);

    show_map(0, true);
    ssd1306_draw();

    sleep_ms(20000);
    // TODO
    // Exit on button press OR timer
}


void win() {
    return;
}


void help() {

    ssd1306_clear();
    ssd1306_text(0, 0,  "Phantom Slayer is a", false, false);
    ssd1306_text(0, 8,  "chase game played in", false, false);
    ssd1306_text(0, 16, "a 3D maze. Each maze", false, false);
    ssd1306_text(0, 24, "is inhabited by evil", false, false);
    ssd1306_text(0, 32, "phantoms which can", false, false);
    ssd1306_text(0, 40, "destroy you with a", false, false);
    ssd1306_text(0, 48, "single touch!", false, false);
    ssd1306_text(24, 56, "PRESS A KEY", false, false);
}


/*
 *  Misc Functions
 */
int irandom(int start, int max) {
    // Generate a PRG between 0 and max-1 then add start
    // Eg. 10, 20 -> range 10-29
    return (rand() % max + start);
}


void inkey() {
    // Wait for any button to be pushed
    while (true) {
        if (gpio_get(PIN_TELE_BUTTON) || gpio_get(PIN_FIRE_BUTTON)) {
            uint32_t now = time_us_32();
            if (game.debounce_count_press == 0) {
                game.debounce_count_press = now;
            } else if (now - game.debounce_count_press > DEBOUNCE_TIME_US) {
                game.debounce_count_press == 0;
                break;
            }
        }
    }
}


void tone(unsigned int frequency, unsigned long duration, unsigned long post) {
    // Get the cycle period in microseconds
    // NOTE Input is in Hz
    float period = 1000000 / (float)frequency;

    // Get the microsecond timer now
    unsigned long start = time_us_64();

    // Loop until duration (ms) in microseconds has elapsed
    while (time_us_64() < start + duration * 1000) {
        gpio_put(PIN_SPEAKER, true);
        sleep_us(0.5 * period);
        gpio_put(PIN_SPEAKER, false);
        sleep_us(0.5 * period);
    };

    // Apply a post-tone delay
    sleep_ms(post);
}


void play_intro() {

    // Display the opening titles
    int8_t final_y = 0;
    bool sstate = true;

    for (int8_t i = -8 ; i < 22 ; ++i) {
        ssd1306_clear();
        ssd1306_text(10, i, "THE PHANTOM SLAYER", false, false);
        ssd1306_draw();
        sleep_ms(25);
        final_y = i;
    }

    for (int8_t i = 64 ; i > 33 ; --i) {
        ssd1306_clear();
        ssd1306_text(10, final_y, "THE PHANTOM SLAYER", false, false);
        ssd1306_text(26, i, "BY TONY SMITH", false, false);
        ssd1306_text(22, i + 10, "AND KEN KALISH", false, false);
        ssd1306_draw();
        sleep_ms(25);
    }

    for (int8_t i = 0 ; i < 10 ; ++i) {
        ssd1306_inverse(sstate);
        sstate = !sstate;
        sleep_ms(200);
    }

    sleep_ms(5000);
}


/*
 * Runtime start
 *
 */
int main() {

    // Setup the hardware
    setup();

    // Play the game
    while (1) {
        // Start a new game
        //play_intro();

        // Set up the environment
        init_game();
        create_world();

        // Clear the screen and present the current map
        ssd1306_clear();
        show_map(0, false);
        ssd1306_draw();
        sleep_ms(10000);

        // Show the world...
        ssd1306_inverse(true);
        update_world(time_us_32());

        // ...and start play
        game_loop();
    }

    return 0;
}
