/*
 * Phantom Slayer
 *
 * @version     1.0.2
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _GAME_UTILS_HEADER_
#define _GAME_UTILS_HEADER_


/*
 * PROTOYPES
 */
int     irandom(int start, int max);
uint8_t inkey();
void    tone(unsigned int frequency_hz, unsigned long duration_ms, unsigned long post_play_delay_ms);



#endif  // _GAME_UTILS_HEADER_