/*
 * ski for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _SSD1306_CLASS_HEADER_
#define _SSD1306_CLASS_HEADER_


/*
 * CONSTANTS
 */
// SSD1306 Addresses
#define  SSD1306_ADDRESS_128x32                          0x3C
#define  SSD1306_ADDRESS_128x64                          0x3D

// SSD1306 Instructions
#define  SSD1306_DO_CMD                                  0x00
#define  SSD1306_WRITE_TO_BUFFER                         0x40

// SSD1306 Registers
#define  SSD1306_MEMORYMODE                              0x20
#define  SSD1306_COLUMNADDR                              0x21
#define  SSD1306_PAGEADDR                                0x22
#define  SSD1306_SETSTARTLINE                            0x40
#define  SSD1306_SETCONTRAST                             0x81
#define  SSD1306_CHARGEPUMP                              0x8D
#define  SSD1306_SEGREMAP                                0xA1
#define  SSD1306_DISPLAYALLON_RESUME                     0xA4
#define  SSD1306_DISPLAYALLON                            0xA5
#define  SSD1306_NORMALDISPLAY                           0xA6
#define  SSD1306_INVERTDISPLAY                           0xA7
#define  SSD1306_SETMULTIPLEX                            0xA8
#define  SSD1306_DISPLAYOFF                              0xAE
#define  SSD1306_DISPLAYON                               0xAF
#define  SSD1306_COMSCANDEC                              0xC8
#define  SSD1306_SETDISPLAYOFFSET                        0xD3
#define  SSD1306_SETDISPLAYCLOCKDIV                      0xD5
#define  SSD1306_SETPRECHARGE                            0xD9
#define  SSD1306_SETCOMPINS                              0xDA
#define  SSD1306_SETVCOMDETECT                           0xDB


/*
 * THE CLASS
 */
class SSD1306 {

    private:
    // PROTOTYPES
    void        setup();
    uint16_t    coords_to_index(uint8_t x, uint8_t y);
    void        write(bool is_command, uint8_t *data, uint16_t data_size);
    void        i2c_write_byte(uint8_t byte);
    void        i2c_write_block(uint8_t *data, uint16_t count);
    uint8_t     text_flip(uint8_t value);
    uint16_t    text_stretch(uint8_t x);
    void        plot_char(int8_t x, int8_t y, uint8_t char_bit, uint16_t char_byte, uint8_t byte_bit);

    // PROPERTIES
    uint8_t     buffer[1152];       // Hack for this application ONLY:
                                    // add an extra row of bytes
    uint8_t     i2c_buffer[1025];
    uint8_t     screen_width;
    uint8_t     screen_height;
    uint16_t    buffer_size;
    uint16_t    i2c_buffer_size;
    uint8_t     i2c_address;
    bool        is_inverted;
    bool        has_hidden_row;


    public:
    // PROTOTYPES
                SSD1306(uint8_t pixel_width, uint8_t pixel_height, bool extra_row);
    void        plot(int8_t x, int8_t y, uint8_t colour);
    void        rect(int8_t x, int8_t y, uint8_t width, uint8_t height, uint8_t colour, bool fill);
    void        line(int8_t x, int8_t y, int8_t tox, int8_t toy, uint8_t colour, uint8_t thick);
    void        text(int8_t x, int8_t y, string the_string, bool do_wrap, bool do_double);
    void        nibble_scroll();
    void        bit_scroll();
    void        clear();
    void        draw();
    void        inverse(bool do_invert);
};


#endif  // _SSD1306_CLASS_HEADER_