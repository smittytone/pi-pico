/*
 * Planet Invasion for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _PLANET_INVASION_ENTITIES_HEADER_
#define _PLANET_INVASION_ENTITIES_HEADER_


/*
 * PROTOTYPES
 */
void        move_crystals();
uint8_t     live_crystal_count();
uint8_t     closest_crystal(int16_t x);

void        move_bullets();
bool        fire_bullet(int16_t x, int8_t y);


#endif  // _PLANET_INVASION_ENTITIES_HEADER_
