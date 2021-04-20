/*
 * ski for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _SKI_UTILS_HEADER_
#define _SKI_UTILS_HEADER_

/*
 * CONSTANTS
 */
#define KEY_ERROR_CONDITION             -1

/*
 * PROTOTYPES
 */
int         irandom(int start, int max);
int8_t      inkey();
void        tone(unsigned int frequency_hz, unsigned long duration_ms, unsigned long post_play_delay_ms);


#endif  // _SKI_UTILS_HEADER_