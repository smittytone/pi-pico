/*
 * cellular::ht16k33_driver for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _HT16K33_HEADER_
#define _HT16K33_HEADER_


/*
 * CONSTANTS
 */
#define HT16K33_GENERIC_DISPLAY_ON          0x81
#define HT16K33_GENERIC_DISPLAY_OFF         0x80
#define HT16K33_GENERIC_SYSTEM_ON           0x21
#define HT16K33_GENERIC_SYSTEM_OFF          0x20
#define HT16K33_GENERIC_DISPLAY_ADDRESS     0x00
#define HT16K33_GENERIC_CMD_BRIGHTNESS      0xE0
#define HT16K33_GENERIC_CMD_BLINK           0x81
#define HT16K33_ADDRESS                     0x70

#define HT16K33_SEGMENT_COLON_ROW           0x04
#define HT16K33_SEGMENT_MINUS_CHAR          0x10
#define HT16K33_SEGMENT_DEGREE_CHAR         0x11
#define HT16K33_SEGMENT_SPACE_CHAR          0x00


/*
 * CLASSES
 */
class HT16K33_Segment {

    public:
        HT16K33_Segment(uint32_t address = HT16K33_ADDRESS);

        void                init();
        void                power_on(bool turn_on = true);

        void                set_brightness(uint32_t brightness = 15);
        HT16K33_Segment&    set_colon(bool is_set = false);
        HT16K33_Segment&    set_glyph(uint32_t glyph, uint32_t digit, bool has_dot = false);
        HT16K33_Segment&    set_number(uint32_t number, uint32_t digit, bool has_dot = false);
        HT16K33_Segment&    set_alpha(char chr, uint32_t digit, bool has_dot = false);

        HT16K33_Segment&    clear();
        void                draw();


    private:
        uint8_t     buffer[16];
        uint32_t    pos[4];
        uint32_t    i2c_addr;

};


#endif  // _HT16K33_HEADER_