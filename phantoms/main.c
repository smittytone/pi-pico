/*
 * Phantom Slayer
 *
 * @version     1.0.2
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

    // Randomise using TinyMT
    // https://github.com/MersenneTwister-Lab/TinyMT
    tinymt32_init(&tinymt_store, adc_read());

    // Set up the LED to flash with the speaker
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    gpio_put(PIN_LED, false);

    // Make the graphic frame rects
    // NOTE These are pixel values:
    //      left, top, width, height, Phantom lateral offset
    uint8_t coords[] = { 0,  0, 128, 64, 44,     // Outer LED frame
                        11,  5, 106, 54, 31,
                        24, 10,  80, 44, 21,
                        36, 15,  56, 34, 14,
                        47, 20,  34, 24,  8,
                        55, 25,  18, 14,  4,
                        61, 27,   6, 10, 63};    // 'End wall' for distant views

    // Read the array values into Rect structures
    uint8_t count = 0;
    for (uint8_t i = 0 ; i < sizeof(coords) ; i += 5) {
        Rect a_rect;
        a_rect.x = coords[i];
        a_rect.y = coords[i + 1];
        a_rect.width = coords[i + 2];
        a_rect.height = coords[i + 3];
        a_rect.spot = coords[i + 4];
        rects[count++] = a_rect;
    }

    // Set the address of the buffer we'll draw to
    draw_buffer = &oled_buffer[0];
}


void init_game() {
    // Reset the main game control structure
    game.in_play = false;
    game.show_reticule = false;
    game.is_firing = false;
    game.can_teleport = false;
    game.is_joystick_centred = true;
    game.show_compass = false;
    game.level_score = 0;
    game.audio_range = 4;
    game.phantoms = 1;
    game.level = 1;
    game.zap_time = 0;
    game.debounce_count_press = 0;
    game.debounce_count_release = 0;
    game.tele_x = 0;
    game.tele_y = 0;
    game.start_x = 0;
    game.start_y = 0;
    game.phantom_speed = PHANTOM_MOVE_TIME_US << 1;

    // If these demo/test modes are both set,
    // chase mode takes priority
    chase_mode = false;
    map_mode = false;
}


void init_phantoms() {
    // Reset the array stored phantoms structures
    for (uint8_t i = 0 ; i < 3 ; ++i) {
        Phantom* p = &phantoms[i];
        p->x = ERROR_CONDITION;
        p->y = ERROR_CONDITION;
        p->hp = 1;
        p->hp_max = 1;
        p->direction = 0;
        p->back_steps = false;
    }
}


void create_world() {
    // Generate and populate a new maze which happens
    // at the start of a new game and at the start of
    // each level. A level jump is triggered when all the
    // current phantoms have been dispatched

    // Reset the game
    if (game.level > 0) init_game();
    game.in_play = true;

    // Initialise the current map
    map_init();
    last_draw = 0;

    // Set the teleport
    while (true) {
        // Pick a random co-ordinate
        uint8_t x = irandom(0, 20);
        uint8_t y = irandom(0, 20);

        if (get_square_contents(x, y) == MAP_TILE_CLEAR) {
            game.tele_x = x;
            game.tele_y = y;
            break;
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
    game.start_x = x;
    game.start_y = y;

    // Reset the the phantoms data
    init_phantoms();

    // Add the first phantom to the map, everywhere but empty
    // or where the player
    while (true) {
        // Pick a random co-ordinate
        uint8_t x = irandom(0, 20);
        uint8_t y = irandom(0, 20);

        // If the chosen square is valid, use it
        if ((x < player_x - 4 || x > player_x + 4) && (y < player_y - 4 || y > player_y + 4)) {
            if (get_square_contents(x, y) == MAP_TILE_CLEAR) {
                phantoms[0].x = x;
                phantoms[0].y = y;
                break;
            }
        }
    }

    /* TEST DATA
    player_x = 0;
    player_y = 0;
    player_direction = 1;

    game.phantoms = 3;
    phantoms[0].x = 8;
    phantoms[0].y = 0;

    phantoms[1].x = 9;
    phantoms[1].y = 0;

    phantoms[2].x = 11;
    phantoms[2].y = 0;
     */
}


void game_loop() {
    // Loop while we're in play on one game
    // Death will cause us to break out to the main
    // game-to-game loop
    do {
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

                if (ny < 20 && nx < 20 && get_square_contents(nx, ny) != MAP_TILE_WALL) {
                    // Has the player walked up to a Phantom?
                    if (locate_phantom(nx, ny) != ERROR_CONDITION) {
                        // Yes -- so the player is dead!
                        game.in_play = false;
                    }

                    // Set the new square for rendering later
                    player_x = nx;
                    player_y = ny;
                }
            } else if (dir == TURN_RIGHT) {
                // Turn player right
                ++player_direction;
                if (player_direction > DIRECTION_WEST) player_direction = DIRECTION_NORTH;

                // Animate the turn now
                if (!chase_mode && !map_mode) animate_turn(false);
            } else if (dir == TURN_LEFT) {
                // Turn player left
                --player_direction;
                if (player_direction > DIRECTION_WEST) player_direction = DIRECTION_WEST;

                // Animate the turn now
                if (!chase_mode && !map_mode) animate_turn(true);
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
                    // Prime the laser if it's unprimed, ie. show the crosshair
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
                    // Fire the laser: clear the cross hair and zap
                    game.debounce_count_release == 0;
                    game.show_reticule = false;
                    game.is_firing = true;
                    game.can_fire = false;
                    game.zap_time = time_us_32();
                }
            }
        }

        if (gpio_get(PIN_TELE_BUTTON)) {
            // Player can only teleport if they have walked over the
            // teleport square and they are not firing the laser
            if (!game.show_reticule) {
                if (game.debounce_count_press == 0) {
                    // Set debounce timer
                    game.debounce_count_press = now;
                } else if (now - game.debounce_count_press > DEBOUNCE_TIME_US) {
                    // Teleport
                    game.debounce_count_press == 0;
                    if (player_x == game.tele_x && player_y == game.tele_y) {
                        do_teleport();
                    }
                }
            }
        }

        // Manage and draw the world
        update_world(time_us_32());

        if (game.in_play && game.is_firing) {
            game.is_firing = false;
            fire_laser();
        }

    // At the end of the loop check to see if the player is
    // still alive -- and drop out if they are not
    } while (game.in_play);

    // Show the death view when the game loop exits
    death();

    // At this point, the code returns to the outer loop
    // in 'main()' to initialize a new game and start it
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


void update_world(uint32_t now) {
    // Update the world at the end of the move cycle
    // Draw the graphics and animate the phantoms

    // Move the Phantoms periodically -- this is how
    // we increase their speed as the game progresses
    if (now - last_phantom_move > game.phantom_speed) {
        last_phantom_move = now;
        move_phantoms();
        check_senses();
    }

    // Draw the world periodically (approx. 44fps)
    if (now - last_draw > ANIM_TIME_US || !game.in_play) {
        ssd1306_clear();

        // Render the screen
        if (chase_mode) {
            draw_screen(phantoms[0].x, phantoms[0].y, phantoms[0].direction);
        } else if (map_mode) {
            show_map(0, true);
        } else {
            draw_screen(player_x, player_y, player_direction);
        }

        // Has the player primed the laser? If so show the crosshair
        if (game.show_reticule) {
            // White outline
            ssd1306_rect(62, 24, 4, 7, 0, false);
            ssd1306_rect(62, 33, 4, 7, 0, false);
            ssd1306_rect(55, 30, 7, 4, 0, false);
            ssd1306_rect(66, 30, 7, 4, 0, false);

            // Black inner bars
            ssd1306_rect(63, 25, 2, 5, 1, false);
            ssd1306_rect(63, 34, 2, 5, 1, false);
            ssd1306_rect(56, 31, 5, 2, 1, false);
            ssd1306_rect(67, 31, 5, 2, 1, false);
        }

        // **** EXPERIMENTAL ****
        if (game.show_compass) {
            ssd1306_circle(122, 5, 10, 0, false);
            ssd1306_circle(122, 5, 8, 1, true);

            switch(player_direction) {
                case DIRECTION_NORTH:
                case DIRECTION_SOUTH:
                    ssd1306_line(122,2,122,8,0,1);
                    break;
                default:
                    ssd1306_line(129,5,125,5,0,1);
            }

            switch(player_direction) {
                case DIRECTION_NORTH:
                    ssd1306_line(121,3,123,3,0,1);
                    break;
                case DIRECTION_SOUTH:
                    ssd1306_line(121,7,123,7,0,1);
                    break;
                case DIRECTION_EAST:
                    ssd1306_line(124,4,124,6,0,1);
                    break;
                default:
                    ssd1306_line(120,4,120,6,0,1);
            }

        }

        // Send the rendered image to the display
        ssd1306_draw();
        last_draw = now;
    }

    // Check for a laser recharge
    if (now - game.zap_time > LASER_RECHARGE_US) {
        game.zap_time = 0;
        game.can_fire = true;
    }
}


/*
 *  Actions
 */
void check_senses() {
    // Scan around the player for nearby phantoms
    int8_t dx = player_x - game.audio_range;
    int8_t dy = player_y - game.audio_range;

    for (int8_t i = dx ; i < dx + (game.audio_range << 1) ; ++i) {
        if (i < 0) continue;
        if (i > 19) break;
        for (int8_t j = dy ; j < dy + (game.audio_range << 1) ; ++j) {
            if (j < 0) continue;
            if (j > 19) break;
            if (locate_phantom(i, j) != ERROR_CONDITION) {
                // There's a Phantom in range, so
                // flash the LED and sound a tone
                gpio_put(PIN_LED, true);
                tone(200, 10, 0);
                gpio_put(PIN_LED, false);

                // Only play one beep, no matter
                // how many nearby phantoms there are
                return;
            }
        }
    }
}


void do_teleport() {
    // Jump back to the teleport square
    // if the player has walked over it

    // Flash the screen
    bool tstate = false;
    for (uint8_t i = 0 ; i < 10 ; i++) {
        ssd1306_inverse(tstate);
        tone((tstate ? 4000 : 2000), 100, 0);
        tstate = !tstate;
    }

    // Move the player to the stored square
    player_x = game.start_x;
    player_y = game.start_y;
}


void fire_laser() {
    // Hit the front-most facing phantom, if there is one

    // Set the zap circle radii
    uint8_t radii[] = {20, 16, 10, 4};

    // Copy the current display buffer contents for use
    // in the animation
    memcpy(&temp_buffer[0], oled_buffer, oled_buffer_size);

    // Draw a sequence of smaller circles as the zap
    // moves away from the player
    for (uint8_t i = 0 ; i < 4 ; ++i) {
        // Laser shot is a white circle with a black fill
        ssd1306_circle(64, 32, radii[i], 0, false);
        ssd1306_circle(64, 32, radii[i] - 1, 1, true);

        // Send the buffer to the display and sound a beep
        ssd1306_draw();
        tone(4800, 40, 40);

        // Copy the saved, clear screen back to the buffer
        // ready for the next animation frame's draw cycle
        memcpy(&oled_buffer[0], temp_buffer, oled_buffer_size);
    }

    // Draw in the final bulletless view
    ssd1306_draw();

    // Did we hit a Phantom?
    uint8_t n = get_facing_phantom(MAX_VIEW_RANGE);
    if (n != ERROR_CONDITION) {
        // A hit! A palpable hit!
        // Deduct 1HP from the Phantom
        Phantom* p = &phantoms[n];
        p->hp -= 1;

        // FROM 1.0.2
        // Use original scoring: 2 points for a hit, 10 for a kill
        game.level_score += 2;

        // Did that kill it?
        if (p->hp == 0) {
            // Yes! One dead Phantom...
            game.level_score += 10;
            ++game.level_kills;

            // Briefly invert the screen and sound some tones
            ssd1306_inverse(false);
            tone(1200, 100, 200);
            draw_screen(player_x, player_y, player_direction);
            tone(600, 100, 200);

            // Quickly show the map
            ssd1306_clear();
            show_scores();
            sleep_ms(MAP_POST_KILL_SHOW_MS);

            // Take the dead phantom off the board
            // (so it gets re-rolled in 'managePhantoms()')
            p->x = ERROR_CONDITION;
            p->y = ERROR_CONDITION;

            // Draw without the zapped Phantom
            ssd1306_clear();
            draw_screen(player_x, player_y, player_direction);
            ssd1306_inverse(true);
            ssd1306_draw();
        }
    }

    // Update phantoms list
    manage_phantoms();
}


/*
 * Game Outcomes
 */
void death() {
    // The player has died -- show the map and the score
    for (unsigned int i = 400 ; i > 100 ; i -= 2) tone(i, 30, 0);
    sleep_ms(50);
    tone(2200, 500, 600);

    ssd1306_clear();
    ssd1306_text(0, 0, "YOU", false, false);
    ssd1306_text(0, 8, "WERE", false, false);
    ssd1306_text(0, 16, "KILLED", false, false);

    ssd1306_text(0, 40, "PRESS", false, false);
    ssd1306_text(0, 48, "ANY", false, false);
    ssd1306_text(0, 56, "KEY", false, false);

    // Show the map
    show_scores();
    ssd1306_inverse(false);

    // Wait for a key press
    inkey();
}


void show_scores() {
    // Code used in a couple of 'show map' locations
    // Show the current score alongside the map
    char score_string[5] = "000";
    ssd1306_text(98, 0, "SCORE", false, false);
    sprintf(score_string, "%02d", game.level_score);
    ssd1306_text(98, 9, score_string, false, (game.level_score < 100));

    // Show the high score
    ssd1306_text(98, 32, "HIGH", false, false);
    ssd1306_text(98, 40, "SCORE", false, false);

    if (high_score < game.level_score) high_score = game.level_score;
    sprintf(score_string, "%02d", high_score);
    ssd1306_text(98, 49, score_string, false, (high_score < 100));

    show_map(0, true);
    ssd1306_draw();
}


void play_intro() {
    // Display the opening title sequence
    int8_t final_y = 0;
    int8_t a_tone = 0;
    bool sstate = true;

    // Draw the animated text blocks, one after the other
    for (int8_t i = -8 ; i < 24 ; ++i) {
        ssd1306_clear();
        ssd1306_text(10, i, "THE PHANTOM SLAYER", false, false);
        ssd1306_draw();
        tone((((a_tone++) + 16) << 2), 40, 0);
        final_y = i;
    }

    for (int8_t i = 64 ; i > 32 ; --i) {
        ssd1306_clear();
        ssd1306_text(10, final_y, "THE PHANTOM SLAYER", false, false);
        ssd1306_text(26, i, "BY TONY SMITH", false, false);
        ssd1306_text(29, i + 10, "& KEN KALISH", false, false);
        ssd1306_draw();
        tone(((a_tone++) << 2), 40, 0);
    }

    // Flash the screen by inverting
    for (int8_t i = 0 ; i < 10 ; ++i) {
        ssd1306_inverse(sstate);
        sstate = !sstate;
        for (int8_t j = 0 ; j < 20 ; ++j) {
            tone(irandom(100, 500), 5, 0);
        }
        sleep_ms(300);
    }

    // Wait 5 seconds
    sleep_ms(5000);
}


/*
 * Runtime start
 *
 */
    int main() {

    // Setup the hardware
    setup();

    // FROM 1.0.2
    show_help();

    // Start a new game
    play_intro();

    // Play the game
    while (1) {
        // Set up the environment, once per game
        init_game();
        create_world();

        // Clear the screen, present the current map and
        // give the player a five-second countdown
        char count_string[2] = "00";
        for (uint8_t i = 5 ; i > 0 ; --i) {
            ssd1306_clear();
            ssd1306_text(13, 0, "NEW", false, false);
            ssd1306_text(7, 8, "GAME", false, false);
            ssd1306_text(98, 0, "LEVEL", false, false);
            ssd1306_text(108, 8, "1", false, true);

            sprintf(count_string, "%d", i);
            ssd1306_text(10, 48, count_string, false, true);

            show_map(0, false);
            ssd1306_draw();
            tone(2200, 20, 0);
            sleep_ms(1000);
        }

        // Show the world...
        update_world(time_us_32());
        ssd1306_inverse(true);

        // ...and start play
        game_loop();
    }

    return 0;
}
