/*
 * Generic Game Utilities for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
 #include "invasion.h"


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
    uint8_t the_key = 255;
    bool is_pressed = false;
    bool was_pressed = false;

    while (true) {
        uint32_t now = time_us_32();

        if (gpio_get(PIN_BOMB_BUTTON)) {
            the_key = PIN_BOMB_BUTTON;
            is_pressed = true;
        } else if (gpio_get(PIN_FIRE_BUTTON)) {
            the_key = PIN_FIRE_BUTTON;
            is_pressed = true;
        } else {
            is_pressed = false;
        }

        if (is_pressed) {
            if (timers[TIMER_INKEY_DEBOUNCE] == 0) {
                timers[TIMER_INKEY_DEBOUNCE] = now;
            } else if (now - timers[TIMER_INKEY_DEBOUNCE] > DEBOUNCE_TIME_US) {
                timers[TIMER_INKEY_DEBOUNCE] == 0;
                was_pressed = true;
            }
        } else if (was_pressed) {
            // Button released
            if (timers[TIMER_INKEY_DEBOUNCE] == 0) {
                // Set debounce timer
                timers[TIMER_INKEY_DEBOUNCE] = now;
            } else if (now - timers[TIMER_INKEY_DEBOUNCE] > DEBOUNCE_TIME_US) {
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


void* alloc(size_t n) {
    // Get a generic pointer, check it's not NULL,
    // and return it if it's good -- exit otherwis
    void *ptr = malloc(n);
    if (ptr == NULL) exit(1);
    return ptr;
}