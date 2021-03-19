/*
 * Phantoms
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#include "main.h"


void play_death_march() {
    // Play the death music
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
}