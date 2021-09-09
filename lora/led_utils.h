/*
 * lora::led_utils for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _LED_HEADER_
#define _LED_HEADER_


/*
 * CONSTANTS
 */
#define PIN_LED                 25


/*
 * PROTOTYPES
 */
namespace LED {
    void            setup();
    void            on();
    void            off();
    void            blink(uint32_t blinks);
}


#endif  // _LED_HEADER_