/*
 * Phantom Slayer
 *
 * @version     1.0.3
 * @author      smittytone
 * @copyright   2024, Tony Smith
 * @licence     MIT
 *
 */
#include "main.h"


extern Game game;
extern tinymt32_t tinymt_store;


int irandom(int start, int max) {
    // Randomise using TinyMT
    // https://github.com/MersenneTwister-Lab/TinyMT
    // Generate a PRG between 0 and max-1 then add start
    // Eg. 10, 20 -> range 10-29
    uint32_t value = tinymt32_generate_uint32(&tinymt_store);
    return (value % max + start);
}


uint8_t inkey() {
    // Wait for any button to be pushed, using debounce

    // FROM 1.0.2
    // Return the key pressed
    uint8_t the_key = ERROR_CONDITION;
    bool is_pressed = false;
    bool was_pressed = false;

    while (true) {
        uint32_t now = time_us_32();

        if (gpio_get(PIN_TELE_BUTTON)) {
            the_key = PIN_TELE_BUTTON;
            is_pressed = true;
        } else if (gpio_get(PIN_FIRE_BUTTON)) {
            the_key = PIN_FIRE_BUTTON;
            is_pressed = true;
        } else {
            is_pressed = false;
        }

        if (is_pressed) {
            if (game.debounce_count_press == 0) {
                game.debounce_count_press = now;
            } else if (now - game.debounce_count_press > DEBOUNCE_TIME_US) {
                game.debounce_count_press == 0;
                was_pressed = true;
            }
        } else if (was_pressed) {
            // Button released
            if (game.debounce_count_press == 0) {
                // Set debounce timer
                game.debounce_count_press = now;
            } else if (now - game.debounce_count_press > DEBOUNCE_TIME_US) {
                // Fire the laser: clear the cross hair and zap
                return the_key;
            }
        }
    }

    return the_key;
}


void tone(unsigned int frequency_hz, unsigned long duration_ms, unsigned long post_play_delay_ms) {
    // Play the required tone on the pizeo speaker pin

    // Get the cycle period in microseconds
    // NOTE Input is in Hz
    float period = 1000000 / (float)frequency_hz;

    // Get the microsecond timer now
    unsigned long start = time_us_64();

    // Loop until duration (milliseconds) in microseconds has elapsed
    while (time_us_64() < start + duration_ms * 1000) {
        gpio_put(PIN_SPEAKER, true);
        sleep_us(0.5 * period);
        gpio_put(PIN_SPEAKER, false);
        sleep_us(0.5 * period);
    };

    // Apply a post-tone delay
    if (post_play_delay_ms > 0) sleep_ms(post_play_delay_ms);
}
