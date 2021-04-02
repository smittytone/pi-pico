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
    uint8_t the_key;

    while (true) {
        if (gpio_get(PIN_TELE_BUTTON)) {
            the_key = PIN_TELE_BUTTON;
        } else if (gpio_get(PIN_FIRE_BUTTON)) {
            the_key = PIN_FIRE_BUTTON;
        } else {
            the_key = ERROR_CONDITION;
        }

        if (the_key != ERROR_CONDITION) {
            uint32_t now = time_us_32();
            if (game.debounce_count_press == 0) {
                game.debounce_count_press = now;
            } else if (now - game.debounce_count_press > DEBOUNCE_TIME_US) {
                game.debounce_count_press == 0;
                return the_key;
            }
        }
    }
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