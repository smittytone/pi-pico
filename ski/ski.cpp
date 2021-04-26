/*
 * ski for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "ski.h"

using std::string;


/*
 *  Initialisation Functions
 */
void setup() {
    // Set up the game hardware

    // Set up the speaker
    gpio_init(PIN_SPEAKER);
    gpio_set_dir(PIN_SPEAKER, GPIO_OUT);
    gpio_put(PIN_SPEAKER, false);

    // Set up the Turn button
    gpio_init(PIN_TURN_BUTTON);
    gpio_set_dir(PIN_TURN_BUTTON, GPIO_IN);
    gpio_pull_down(PIN_TURN_BUTTON);

    // Set up the Pause button
    gpio_init(PIN_PAUSE_BUTTON);
    gpio_set_dir(PIN_PAUSE_BUTTON, GPIO_IN);
    gpio_pull_down(PIN_PAUSE_BUTTON);

    // Use one of the Pico's other analog inputs
    // to seed the random number generator
    adc_init();
    adc_gpio_init(28);
    adc_select_input(2);
    tinymt32_init(&tinymt_store, adc_read());
}


Game init_game() {
    // Set initial values
    Game game = {
        .player_x = (SCREEN_WIDTH >> 1),
        .delta_x  = 1,
        .level    = 1,
        .metres   = 0,
        .speed    = 60,
        .ghost    = false,
        .paused   = false
    };

    return game;
}


void game_loop(SSD1306 &display) {

    display.clear();
    display.draw();

    Game game = init_game();

    uint32_t debounce_count_press[2] = {0, 0};
    bool     was_pressed[2] = {false, false};

    bool     game_in_play = true;
    uint32_t anim_time = 0;
    uint8_t  gap_count, tree_pos, prize_off_screen, x_old, ghost_count = 0;
    char     readout[5];

    while (true) {
        if (!game.paused) {
            display.bit_scroll();

            // Draw in the top line with game details
            display.rect(0, 0, 30, 8, 0, true);
            sprintf(readout, "%05d", game.metres);
            display.text(0, 0, readout, false, false);

            // Crash?
            // TODO

            if (!game_in_play) break;

            // Preserve previous location
            x_old = game.player_x;

            // Check for button presses
            uint32_t now = time_us_32();
            if (gpio_get(PIN_TURN_BUTTON)) {
                // Button pressed
                if (debounce_count_press[0] == 0) {
                    debounce_count_press[0] = now;
                } else if (now - debounce_count_press[0] > DEBOUNCE_TIME_US) {
                    debounce_count_press[0] == 0;
                    was_pressed[0] = true;
                }
            } else if (was_pressed[0]) {
                // Button released
                if (debounce_count_press[0] == 0) {
                    // Set debounce timer
                    debounce_count_press[0] = now;
                } else if (now - debounce_count_press[0] > DEBOUNCE_TIME_US) {
                    // Player switches direction
                    game.delta_x *= -1;
                    was_pressed[0] = false;
                }
            }

            if (gpio_get(PIN_PAUSE_BUTTON)) {
                // Button pressed
                if (debounce_count_press[1] == 0) {
                    debounce_count_press[1] = now;
                } else if (now - debounce_count_press[1] > DEBOUNCE_TIME_US) {
                    debounce_count_press[1] == 0;
                    was_pressed[1] = true;
                }
            } else if (was_pressed) {
                // Button released
                if (debounce_count_press[1] == 0) {
                    // Set debounce timer
                    debounce_count_press[1] = now;
                } else if (now - debounce_count_press[1] > DEBOUNCE_TIME_US) {
                    // Player switches direction
                    game.paused = !game.paused;
                    was_pressed[1] = false;
                }
            }

            // Move the player
            game.player_x += game.delta_x;

            // Bounce off the edges
            if (game.player_x < 1) {
                game.player_x = 2;
                game.delta_x = 1;
            }

            if (game.player_x > SCREEN_WIDTH - 4) {
                game.player_x = SCREEN_WIDTH - 5;
                game.delta_x = -1;
            }

            // Draw the player
            //display.line(x_old + 1, PLAYER_Y - 1, x_old + 4, PLAYER_Y - 1, 0, 1);
            display.plot(game.player_x,     PLAYER_Y    , 1);
            display.plot(game.player_x + 4, PLAYER_Y    , 1);
            display.plot(game.player_x + 1, PLAYER_Y + 1, 1);
            display.plot(game.player_x + 3, PLAYER_Y + 1, 1);
            display.plot(game.player_x + 2, PLAYER_Y + 2, 1);

            // Do we need to draw a way marker?
            if ((game.metres != 0) && (game.metres % 1000 == 0)) {
                ++game.level;
                game.speed = 40 - (5 * game.level);
                if (game.speed < 0) game.speed = 0;
                for (uint8_t i = 0 ; i < 127 ; i += 2) {
                    display.plot(i,     64, 1);
                    display.plot(i + 1, 65, 1);
                    display.plot(i,     66, 1);
                    display.plot(i + 1, 67, 1);
                }
            } else {
                // Draw up to three tress per level
                if (gap_count >= 8) {
                    for (uint8_t i = 0 ; i < 3 ; ++i) {
                        tree_pos = irandom(0, 101);
                        if (tree_pos > (80 - (game.level << 1))) {
                            tree_pos = irandom(0, SCREEN_WIDTH >> 2);
                            draw_tree(tree_pos << 2, display);
                        }
                    }

                    if (!game.ghost && prize_off_screen < 0) {
                        if (tree_pos < 8) {
                            tree_pos = irandom(0, SCREEN_WIDTH >> 2);
                            draw_prize(tree_pos << 2, display);
                        }
                    }

                    gap_count = 0;
                }

                // Place some 'dirt' for colour
                tree_pos = irandom(0, 101);
                if (tree_pos > 80) {
                    tree_pos = irandom(0, SCREEN_WIDTH);
                    display.plot(tree_pos, 63, 1);
                }
            }

            // Screen draw is approx. 20ms
            display.draw();
            //sleep_ms(game.speed);

            ++game.metres;
            ++gap_count;
            ++prize_off_screen;

            if (game.ghost) {
                if (ghost_count > 200) {
                    ghost_count = 0;
                    game.ghost = false;
                } else {
                    ++ghost_count;
                }
            }
        }
    }

    game_in_play = play_again();
}


void draw_tree(uint8_t x, SSD1306 &display) {
    // Draw a tree
    display.plot(x + 2, 64, 1);
    display.line(x + 1, 65, x + 4, 65, 1, 1);
    display.line(x,     66, x + 5, 66, 1, 1);
    display.plot(x + 2, 67, 1);
}


void draw_prize(uint8_t x, SSD1306 &display) {
    // Draw a prize
    display.rect(x,     64, x + 1, 65, 1, false);
    display.rect(x + 2, 66, x + 3, 67, 1, false);
}


bool play_again() {
    return true;
}


int main() {

    // Set up the hardware
    setup();

    // Instantiate the SSD1306 object
    SSD1306 display = SSD1306(128, 64, true);

    // Start the game
    while (true) {
        game_loop(display);
    }

    // Exit gracefully, but we should never reach
    return 0;
}