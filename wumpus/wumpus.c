/*
 * Hunt the Wumpus for Raspberry Pi Pico
 *
 * @version     1.0.2
 * @authors     smittytone, Coreu Faure
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#include "wumpus.h"


/*
 *  Globals
 */
// Wumpus World
char hazards [8][8];
bool visited [8][8];
bool stink_layer[8][8];
bool sound_layer[8][8];
bool draft_layer[8][8];

uint8_t player_x = 0;
uint8_t player_y = 0;
uint8_t last_move_dir = 0;

bool is_in_play = true;
bool is_player_pixel_on = true;

const char text_win[] = "    YOU WIN!    ";
const char text_lose[] = "    YOU DIED!    ";

// Graphics buffer
uint8_t buffer[8];

// Debouncing controls
uint32_t debounce_count_button = 0;
bool is_joystick_centred = true;
uint32_t last_player_pixel_flash = 0;

// TinyMT RNG store
tinymt32_t tinymt_store;


/*
 *  I2C Functions
 */
void i2c_write_byte(uint8_t byte) {
    // Convenience function to write a single byte to the matrix
    i2c_write_blocking(I2C_PORT, HT16K33_ADDRESS, &byte, 1, false);
}

void i2c_write_block(uint8_t *data, uint8_t count) {
    // Convenience function to write a 'count' bytes to the matrix
    i2c_write_blocking(I2C_PORT, HT16K33_ADDRESS, data, count, false);
}


/*
 *  HT16K33 LED Matrix Functions
 */
void ht16k33_init() {
    // Initialize the matrix by powering up
    ht16k33_power_on_or_off(ON);
    ht16k33_set_brightness(2);
    ht16k33_clear();
    ht16k33_draw();
}

void ht16k33_power_on_or_off(uint8_t on) {
    // Power the LED on or off
    i2c_write_byte(on == ON ? HT16K33_GENERIC_SYSTEM_ON : HT16K33_GENERIC_DISPLAY_OFF);
    i2c_write_byte(on == ON ? HT16K33_GENERIC_DISPLAY_ON : HT16K33_GENERIC_SYSTEM_OFF);
}

void ht16k33_set_brightness(uint8_t brightness) {
    // Set the LED brightness
    if (brightness < 0 || brightness > 15) brightness = 15;
    i2c_write_byte(HT16K33_GENERIC_CMD_BRIGHTNESS | brightness);
}

void ht16k33_draw_sprite(const char *sprite) {
    // Write the sprite across the matrix
    // NOTE Assumes the sprite is 8 pixels wide
    for (uint8_t i = 0 ; i < 8 ; ++i) buffer[i] = sprite[i];

    // Send the buffer to the LED matrix
    ht16k33_draw();
}

void ht16k33_draw_sprite_center(const char *sprite) {
    // Write the sprite centred on the screen
    uint8_t width = strlen(sprite);
    uint8_t col = 8 - width;
    col = col >> 1;
    for (uint8_t i = col ; i < width ; ++i) buffer[i] = sprite[i];

    // Send the buffer to the LED matrix
    ht16k33_draw();
}

void ht16k33_plot(uint8_t x, uint8_t y, bool is_set) {
    // Set or unset the specified pixel
    uint8_t col = buffer[x];

    if (is_set) {
        col = col | (1 << y);
    } else {
        col = col & ~(1 << y);
    }

    buffer[x] = col;
}

void ht16k33_print(const char *text) {
    // Scroll the supplied text horizontally across the 8x8 matrix

    // Get the length of the text: the number of columns it encompasses
    uint length = 0;
    for (size_t i = 0 ; i < strlen(text) ; ++i) {
        uint8_t asc_val = text[i] - 32;
        length += (asc_val == 0 ? 2: strlen(CHARSET[asc_val]));
        if (asc_val > 0) length++;
    }

    // Make the output buffer to match the required number of columns
    uint8_t src_buffer[length];
    for (uint i = 0 ; i < length ; ++i) src_buffer[i] = 0x00;

    // Write each character's glyph columns into the output buffer
    uint col = 0;
    for (size_t i = 0 ; i < strlen(text) ; ++i) {
        uint8_t asc_val = text[i] - 32;
        if (asc_val == 0) {
            // It's a space, so just add two blank columns
            col += 2;
        } else {
            // Get the character glyph and write it to the buffer
            uint8_t glyph_len = strlen(CHARSET[asc_val]);

            for (uint j = 0 ; j < glyph_len ; ++j) {
                src_buffer[col] = CHARSET[asc_val][j];
                ++col;
            }

            ++col;
        }
    }

    // Finally, animate the line by repeatedly sending 8 columns
    // of the output buffer to the matrix
    uint cursor = 0;
    while (1) {
        uint a = cursor;
        for (uint8_t i = 0 ; i < 8 ; ++i) {
            buffer[i] = src_buffer[a];
            a += 1;
        }

        ht16k33_draw();
        cursor++;
        if (cursor > length - 8) break;
        sleep_ms(75);
    };
}

void ht16k33_clear() {
    // Clear the display buffer
    for (uint8_t i = 0 ; i < 8 ; ++i) buffer[i] = 0;
}

void ht16k33_draw() {
    // Set up the buffer holding the data to be
    // transmitted to the LED
    uint8_t output_buffer[17];
    for (uint8_t i = 0 ; i < 17 ; ++i) output_buffer[i] = 0;

    // Span the 8 bytes of the graphics buffer
    // across the 16 bytes of the LED's buffer
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        uint8_t a = buffer[i];
        output_buffer[i * 2 + 1] = (a >> 1) + ((a << 7) & 0xFF);
    }

    // Write out the transmit buffer
    i2c_write_block(output_buffer, sizeof(output_buffer));
}


/*
 *  Misc Functions
 */
int irandom(int start, int max) {
    // FROM 1.0.2
    // Randomise using TinyMT rather than rand()
    // https://github.com/MersenneTwister-Lab/TinyMT
    // Generate a PRG between 0 and max-1 then add start
    // Eg. 10, 20 -> range 10-29
    uint32_t value = tinymt32_generate_uint32(&tinymt_store);
    return (value % max + start);
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

    // Set up the LED matrix
    ht16k33_init();

    // Set up sense indicator output pins:
    // Green is the Wumpus nearby indicator
    gpio_init(PIN_GREEN);
    gpio_set_dir(PIN_GREEN, GPIO_OUT);
    gpio_put(PIN_GREEN, false);

    // Red is the Pit nearby indicator
    gpio_init(PIN_RED);
    gpio_set_dir(PIN_RED, GPIO_OUT);
    gpio_put(PIN_RED, false);

    // Set up the speaker
    gpio_init(PIN_SPEAKER);
    gpio_set_dir(PIN_SPEAKER, GPIO_OUT);
    gpio_put(PIN_SPEAKER, false);

    // Set up the Fire button
    gpio_init(PIN_BUTTON);
    gpio_set_dir(PIN_BUTTON, GPIO_IN);
    gpio_pull_down(PIN_BUTTON);

    // Set up the X- and Y-axis joystick input
    adc_init();
    adc_gpio_init(PIN_X);
    adc_gpio_init(PIN_Y);

    // Use one of the Pico's other analog inputs
    // to seed the random number generator
    adc_gpio_init(28);
    adc_select_input(2);
    srand(adc_read());

    // FROM 1.0.2
    // Randomise using TinyMT
    // https://github.com/MersenneTwister-Lab/TinyMT
    tinymt32_init(&tinymt_store, adc_read());
}

void create_world() {
    // Generate the Wumpus' cave.

    // The player starts at (0,0)
    player_x = 0;
    player_y = 0;
    is_in_play = true;

    // Zero the world arrays
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            hazards[i][j] = '#';    // No hazard
            visited[i][j] = false;
            stink_layer[i][j] = false;
            draft_layer[i][j] = false;
            sound_layer[i][j] = false;
        }
    }

    // Create 1-3 bats
    uint8_t bat_x = 0;
    uint8_t bat_y = 0;
    uint8_t number_bats = irandom(1, 4);
    for (uint8_t i = 0; i < number_bats ; ++i) {
        bat_x = irandom(0, 8);
        bat_y = irandom(0, 8);
        hazards[bat_x][bat_y] = 'b';
    }

    // Create 4-8 pits
    uint8_t pit_x = 0;
    uint8_t pit_y = 0;
    uint8_t number_pits = irandom(4, 9);
    for (uint8_t i = 0; i < number_pits; ++i) {
        pit_x = irandom(0, 8);
        pit_y = irandom(0, 8);
        hazards[pit_x][pit_y] = 'p';
    }

    // Create one wumpus
    // NOTE It's generated last so bats and pits
    //      can't overwrite it by chance, and we
    //      make sure it's not in the bottom left
    //      corner
    uint8_t wumpus_x = 0;
    uint8_t wumpus_y = 0;
    while (wumpus_x < 1 && wumpus_y < 1) {
        wumpus_x = irandom(0, 8);
        wumpus_y = irandom(0, 8);
    }

    // Set its location
    hazards[wumpus_x][wumpus_y] = 'w';

    // Make sure the start tile is safe to spawn on
    hazards[0][0] = '#';

    // Generate sense data for sounds and LED reactions
    for (uint8_t i = 0; i < 8; ++i) {
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if (hazards[i][j] == 'w') {
                if (i < 7) stink_layer[i + 1][j] = true;
                if (i > 0) stink_layer[i - 1][j] = true;
                if (j < 7) stink_layer[i][j + 1] = true;
                if (j > 0) stink_layer[i][j - 1] = true;
            } else if (hazards[i][j] == 'p') {
                if (i < 7) draft_layer[i + 1][j] = true;
                if (i > 0) draft_layer[i - 1][j] = true;
                if (j < 7) draft_layer[i][j + 1] = true;
                if (j > 0) draft_layer[i][j - 1] = true;
            } else if (hazards[i][j] == 'b') {
                if (i < 7) sound_layer[i + 1][j] = true;
                if (i > 0) sound_layer[i - 1][j] = true;
                if (j < 7) sound_layer[i][j + 1] = true;
                if (j > 0) sound_layer[i][j - 1] = true;
            }
        }
    }
}


/*
 *  Main Game Loop
 */
void game_loop() {
    // Read the current joystick position.
    // If it's not in the deadzone, then determine
    // which direction it's in (up, down, left or right).
    // If it's in the deadzone, check if the player is trying
    // to fire an arrow.

    while (is_in_play) {
        // Read joystick analog output
        adc_select_input(0);
        uint16_t x = adc_read();
        adc_select_input(1);
        uint16_t y = adc_read();
        bool is_dead = false;

        if (check_joystick(x, y)) {
            // Joystick is pointing in a direction, so
            // get the direction the player has chosen
            uint8_t dir = get_direction(x, y);

            // Record the player's steps before the move
            visited[player_x][player_y] = true;

            if (dir == 0) {
                // Move player up
                if (player_y < 7) {
                    player_y++;
                    last_move_dir = 0;
                }
            } else if (dir == 3) {
                // Move player right
                if (player_x < 7) {
                    player_x++;
                    last_move_dir = 3;
                }
            } else if (dir == 2) {
                // Move player down
                if (player_y > 0) {
                    player_y--;
                    last_move_dir = 2;
                }
            } else {
                // Move player left
                if (player_x > 0) {
                    player_x--;
                    last_move_dir = 1;
                }
            }

            // Check the new location for sense
            // information and hazards
            is_dead = check_hazards();
            if (!is_dead) check_senses();

        } else {
            // Joystick is in deadzone
            if (gpio_get(PIN_BUTTON)) {
                uint32_t now = time_us_32();
                if (debounce_count_button == 0) {
                    // Set debounce timer
                    debounce_count_button = now;
                } else if (now - debounce_count_button > DEBOUNCE_TIME_US) {
                    // Clear debounce timer
                    debounce_count_button == 0;

                    // Shoot arrow
                    fire_arrow_animation();

                    // Did the arrow hit or miss?
                    if (last_move_dir == 0) {
                        if (player_y < 7) {
                            if (hazards[player_x][player_y + 1] == 'w') {
                                dead_wumpus_animation();
                            } else {
                                arrow_miss_animation();
                            }
                            break;
                        }
                    } else if (last_move_dir == 3) {
                        if (player_x < 7) {
                            if (hazards[player_x + 1][player_y] == 'w') {
                                dead_wumpus_animation();
                            } else {
                                arrow_miss_animation();
                            }
                            break;
                        }
                    } else if (last_move_dir == 2) {
                        if (player_y > 0) {
                            if (hazards[player_x][player_y - 1] == 'w') {
                                dead_wumpus_animation();
                            } else {
                                arrow_miss_animation();
                            }
                            break;
                        }
                    } else {
                        if (player_x > 0) {
                            if (hazards[player_x - 1][player_y] == 'w') {
                                dead_wumpus_animation();
                            } else {
                                arrow_miss_animation();
                            }
                            break;
                        }
                    }
                }
            }
        }

        if (!is_dead) {
            // Draw the world then check for smells and hazards
            draw_world();

            // Pause between cycles
            sleep_ms(50);
        }
    }
}


/*
 *  Movement control functions
 */
bool check_joystick(uint16_t x, uint16_t y) {
    // Check to see if the joystick is currently
    // outside of the central deadzone, and that it
    // has returned to the centre before re-reading
    if (x > UPPER_LIMIT || x < LOWER_LIMIT || y > UPPER_LIMIT || y < LOWER_LIMIT) {
        if (is_joystick_centred) {
            // We're good to use the reading, but not
            is_joystick_centred = false;
            return true;
        } else {
            return false;
        }
    }

    // Joystick is centred
    is_joystick_centred = true;
    return false;
}

uint8_t get_direction(uint16_t x, uint16_t y) {
    // Get player direction from the analog input
    if (x < y) {
        if (x > (4096 - y)) {
            return 0;   // up
        } else {
            return 3;   // right
        }
    } else {
        if (x > (4096 - y)) {
            return 1;   // left
        } else {
            return 2;   // down
        }
    }
}

void clear_pins() {
    // Turn off the sense pins no matter what
    gpio_put(PIN_GREEN, false);
    gpio_put(PIN_RED, false);
}


/*
 *  Environment Functions
 */
void draw_world() {
    // Draw the world on the 8x8 LED matrix
    // and blink the player's location.
    ht16k33_clear();
    for(uint8_t i = 0 ; i < 8 ; ++i) {
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            ht16k33_plot(i, j, visited[i][j]);
        }
    }

    // Flash the player's location
    ht16k33_plot(player_x, player_y, is_player_pixel_on);
    ht16k33_draw();

    uint32_t now = time_us_32();
    if (now - last_player_pixel_flash > 250000) {
        is_player_pixel_on = !is_player_pixel_on;
        last_player_pixel_flash = now;
    }
}

void check_senses() {
    // Present the environment to the player
    // Set the smell and draft LEDs
    // Draft = pit, Stench = Wumpus
    gpio_put(PIN_GREEN, stink_layer[player_x][player_y]);
    gpio_put(PIN_RED, draft_layer[player_x][player_y]);

    // Play a sound to signal a nearby bat
    if (sound_layer[player_x][player_y]) {
        tone(600, 50, 50);
        tone(500, 50, 50);
        tone(400, 50, 50);
    }
}

bool check_hazards() {
    // Check to see if player has run into a bat, a pit or the Wumpus
    // If the player steps on a fatal square, 'check_hazards()'
    // returns true, otherwise false
    if (hazards[player_x][player_y] == 'b') {
        // Player encountered a bat: play the animation...
        grabbed_by_bat();

        // ...then drop the player at random
        uint8_t x, y;

        while(1) {
            x = irandom(0, 8);
            y = irandom(0, 8);
            if (hazards[x][y] == '#') break;
        }

        player_x = x;
        player_y = y;
    } else if (hazards[player_x][player_y] == 'p') {
        // Player fell down a pit -- death!
        plunged_into_pit();
        game_lost();
        return true;
    } else if (hazards[player_x][player_y] == 'w') {
        // Player ran into the Wumpus!
        wumpus_win_animation();
        game_lost();
        return true;
    }

    return false;
}


/*
 *  Player events
 */
void grabbed_by_bat() {
    // Show the bat flapping its wings
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        ht16k33_draw_sprite(bat_1);
        sleep_ms(100);
        ht16k33_draw_sprite(bat_2);
        sleep_ms(100);
    }

    // Play the carry animation
    ht16k33_draw_sprite(carry_1);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_2);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_3);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_4);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_5);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_6);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_7);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_8);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_9);
    sleep_ms(100);
}

void plunged_into_pit() {
    // Show the player falling
    ht16k33_draw_sprite(fall_1);
    tone(3000, 100, 100);
    ht16k33_draw_sprite(fall_2);
    tone(2900, 100, 100);
    ht16k33_draw_sprite(fall_3);
    tone(2800, 100, 100);
    ht16k33_draw_sprite(fall_4);
    tone(2700, 100, 100);
    ht16k33_draw_sprite(fall_5);
    tone(2600, 100, 100);
    ht16k33_draw_sprite(fall_6);
    tone(2500, 100, 100);
    ht16k33_draw_sprite(fall_7);
    tone(2400, 100, 100);
    ht16k33_draw_sprite(fall_8);
    tone(2300, 100, 100);
    ht16k33_draw_sprite(fall_9);
    tone(2200, 100, 100);
    ht16k33_draw_sprite(fall_10);
    tone(2100, 100, 100);
    ht16k33_draw_sprite(fall_11);
    tone(2000, 100, 100);
    ht16k33_draw_sprite(fall_12);
    tone(1900, 100, 100);
    ht16k33_draw_sprite(fall_13);
    tone(1800, 100, 100);
    ht16k33_draw_sprite(fall_14);
    tone(1700, 100, 100);
    ht16k33_draw_sprite(fall_15);
    tone(1600, 100, 100);
    ht16k33_draw_sprite(fall_16);
    tone(1500, 100, 100);
    ht16k33_draw_sprite(fall_17);
    tone(1400, 100, 100);
}


/*
 *  Wumpus Attack Animations
 */
void fire_arrow_animation() {
    // Attempt to kill the Wumpus
    // Show arrow firing animation
    sleep_ms(500);
    ht16k33_draw_sprite(bow_1);
    tone(100, 100, 100);
    ht16k33_draw_sprite(bow_2);
    tone(200, 100, 100);
    ht16k33_draw_sprite(bow_3);
    tone(300, 100, 1000);
    ht16k33_draw_sprite(bow_2);

    for (uint8_t i = 0 ; i < 50 ; ++i) {
        tone(irandom(200, 1500), 1, 1);
    }

    ht16k33_draw_sprite(bow_1);

    for (uint8_t i = 0 ; i < 25 ; ++i) {
        tone(irandom(200, 1500), 1, 1);
    }

    ht16k33_draw_sprite(bow_4);
    sleep_ms(50);
    ht16k33_draw_sprite(bow_5);
    sleep_ms(100);
}

void dead_wumpus_animation() {
    // The player successfully kills the Wumpus!
    sleep_ms(500);
    ht16k33_draw_sprite(wumpus_1);
    sleep_ms(500);
    ht16k33_draw_sprite(wumpus_3);
    tone(900, 100, 100);
    ht16k33_draw_sprite(wumpus_4);
    tone(850, 100, 100);
    ht16k33_draw_sprite(wumpus_5);
    tone(800, 100, 100);
    ht16k33_draw_sprite(wumpus_6);
    tone(750, 100, 100);
    ht16k33_draw_sprite(wumpus_7);
    tone(700, 100, 100);
    ht16k33_draw_sprite(wumpus_8);
    tone(650, 100, 100);
    ht16k33_draw_sprite(wumpus_9);
    tone(600, 100, 100);
    ht16k33_draw_sprite(wumpus_10);
    tone(550, 100, 100);
    ht16k33_clear();
    sleep_ms(1000);

    // Success!
    game_won();
}

void arrow_miss_animation() {
    // If the player misses the Wumpus

    // Show the arrow flying past...
    ht16k33_clear();
    sleep_ms(1000);

    for (uint8_t i = 0 ; i < 7 ; i += 2) {
        if (i > 0) ht16k33_plot(i - 2, 4, false);
        ht16k33_plot(i, 4, true);
        ht16k33_draw();
        tone(80, 100, 500);
    }

    // Clear the last arrow point
    ht16k33_clear();
    ht16k33_draw();

    // ...and then the Wumpus gets the player
    wumpus_win_animation();
    game_lost();
}

void wumpus_win_animation() {
    // Player gets attacked from the vicious Wumpus!
    // Complete with nightmare-inducing sound
    for (uint8_t j = 0 ; j < 3 ; ++j) {
        ht16k33_draw_sprite(wumpus_2);
        sleep_ms(250);
        ht16k33_draw_sprite(wumpus_1);
        sleep_ms(250);
    }

    // Play the scream
    for (uint i = 2000 ; i > 800 ; i -= 2) {
        tone(i, 10, 1);
    }

    for (uint8_t j = 0 ; j < 5 ; ++j) {
        ht16k33_draw_sprite(wumpus_2);
        sleep_ms(250);
        ht16k33_draw_sprite(wumpus_1);
        sleep_ms(250);
    }
}


/*
 *  Game Outcomes
 */
void game_won() {
    // Give the player a trophy!
    clear_pins();
    ht16k33_draw_sprite(trophy);
    ht16k33_set_brightness(irandom(1, 15));
    tone(1397, 100, 100);
    ht16k33_set_brightness(irandom(7, 14));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(1, 8));
    tone(1397, 100, 100);
    ht16k33_set_brightness(irandom(7, 14));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(1, 8));
    tone(1397, 100, 100);
    ht16k33_set_brightness(irandom(7, 14));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(1, 8));
    tone(1397, 200, 100);
    ht16k33_set_brightness(irandom(7, 14));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(1, 8));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(7, 14));
    tone(1175, 200, 100);
    ht16k33_set_brightness(irandom(1, 8));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(7, 14));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(1, 8));
    tone(1319, 200, 100);
    ht16k33_set_brightness(irandom(7, 14));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(1, 8));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(7, 14));
    tone(1397, 200, 100);
    ht16k33_set_brightness(irandom(1, 8));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(7, 14));
    sleep_ms(100);
    ht16k33_set_brightness(irandom(1, 8));
    tone(1319, 150, 150);
    ht16k33_set_brightness(irandom(7, 14));
    tone(1397, 400, 100);

    for (uint8_t i = 0 ; i < 6 ; ++i) {
        ht16k33_set_brightness(irandom(1, 8));
        sleep_ms(125);
        ht16k33_set_brightness(irandom(7, 14));
        sleep_ms(125);
    }

    ht16k33_set_brightness(12);
    sleep_ms(1000);
    ht16k33_set_brightness(2);

    // Show the success message
    game_over(text_win);
}

void game_lost() {
    // Give the player a funeral
    clear_pins();
    ht16k33_draw_sprite(grave);
    tone(294, 400, 200);
    tone(294, 400, 200);
    tone(294, 100, 200);
    tone(294, 400, 200);
    tone(349, 400, 200);
    tone(330, 100, 200);
    tone(330, 400, 200);
    tone(294, 100, 200);
    tone(294, 400, 200);
    tone(294, 100, 200);
    tone(294, 800, 3000);

    // Show the failure message
    game_over(text_lose);
}

void game_over(const char *text) {
    // Show final message and
    // clear the screen for the next game
    for (uint8_t i = 0 ; i < 3 ; ++i) ht16k33_print(text);
    is_in_play = false;
    ht16k33_clear();
    ht16k33_draw();
}


/*
 *  The Game's Introduction
 */
void play_intro() {
    // Callback to the theme played in the
    // version by Gregory Yob in 1975.
    // Also show the player entering the cave.
    ht16k33_draw_sprite(begin_1);
    tone(147, 200, 100);    //D3
    ht16k33_draw_sprite(begin_2);
    tone(165, 200, 100);    //E3
    ht16k33_draw_sprite(begin_3);
    tone(175, 200, 100);    //F3
    ht16k33_draw_sprite(begin_4);
    tone(196, 200, 100);    //G3
    ht16k33_draw_sprite(begin_5);
    tone(220, 200, 100);    //A4
    ht16k33_draw_sprite(begin_6);
    tone(175, 200, 100);    //F3
    ht16k33_draw_sprite(begin_7);
    tone(220, 400, 100);    //A4
    ht16k33_draw_sprite(begin_4);
    tone(208, 200, 100);    //G#3
    tone(175, 200, 100);    //E#3
    tone(208, 400, 100);    //G#3
    tone(196, 200, 100);    //G3
    tone(165, 200, 100);    //E3
    tone(196, 400, 100);    //G3
    tone(147, 200, 100);    //D3
    tone(165, 200, 100);    //E3
    tone(175, 200, 100);    //F3
    tone(196, 200, 100);    //G3
    tone(220, 200, 100);    //A3
    tone(175, 200, 100);    //F3
    tone(220, 200, 100);    //A3
    tone(294, 200, 100);    //D4
    tone(262, 200, 100);    //C4
    tone(220, 200, 100);    //A3
    tone(175, 200, 100);    //F3
    tone(220, 200, 100);    //A3
    tone(262, 400, 100);    //C4
}


/*
 *  Runtime start
 */
int main() {
#ifdef TSDEBUG
    // DEBUG control
    stdio_init_all();
#endif

    // Setup the hardware
    setup();

    // Play the game
    while (1) {
        // Set up a new round...
        // Play Wumpus tune
        play_intro();

        // Set up the environment
        create_world();
        draw_world();
        check_senses();

        // ...and start play
        game_loop();
    }

    return 0;
}
