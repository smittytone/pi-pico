/*
 * Generic Game Utilities for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _GENERIC_GAME_UTILS_HEADER_
#define _GENERIC_GAME_UTILS_HEADER_


/*
 * PROTOTYPES
 */
int     irandom(int start, int max);
uint8_t inkey();
void    tone(unsigned int frequency_hz, unsigned long duration_ms, unsigned long post_play_delay_ms);
void*   alloc(size_t n);

/*
 *  CONSTANTS
 */
#define DEBOUNCE_TIME_US                                5000


/*
 * GLOBALS
 */

// Random seed data
tinymt32_t  tinymt_store;


#endif  // _GENERIC_GAME_UTILS_HEADER_