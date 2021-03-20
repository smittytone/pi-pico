/*
 * Phantoms
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021, Tony Smith
 * @licence     MIT
 *
 */
#ifndef _SSD1306_HEADER_
#define _SSD1306_HEADER_


/*
 * PROTOTYPES
 */
// I2C Functions
void     i2c_write_byte(uint8_t byte);
void     i2c_write_block(uint8_t *data, uint16_t count);

// Display Primary Functions
void     ssd1306_init(uint8_t pixel_width, uint8_t pixel_height);
void     ssd1306_inverse(bool do_invert);
void     ssd1306_plot(uint8_t x, uint8_t y, uint8_t colour);
void     ssd1306_line(uint8_t x, uint8_t y, uint8_t tox, uint8_t toy, uint8_t colour, uint8_t thick);
void     ssd1306_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t colour, bool fill);
void     ssd1306_circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t colour, bool fill) ;
void     ssd1306_text(int8_t x, int8_t y, const char *the_string, bool do_wrap, bool do_double);
void     ssd1306_clear();
void     ssd1306_draw();
void     ssd1306_write(bool is_command, uint8_t *data, uint16_t data_size);

// Display Utility Functions
void     ssd1306_char_plot(int8_t x, int8_t y, uint8_t char_bit, uint16_t char_byte, uint8_t byte_bit);
uint8_t  ssd1306_text_flip(uint8_t value);
uint16_t ssd1306_text_stretch(uint8_t x);
uint16_t ssd1306_coords_to_index(uint8_t x, uint8_t y) ;


/*
 * CONSTANTS
 */
// SSD1306 Addresses
#define  SSD1306_ADDRESS_128x32                          0x3C
#define  SSD1306_ADDRESS_128x64                          0x3D

// SSD1306 Instructions
#define  SSD1306_DO_CMD                                  0x00
#define  SSD1306_WRITE_TO_BUFFER                         0x40

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
#define  SSD1306_SETLOWCOLUMN                            0x00
#define  SSD1306_EXTERNALVCC                             0x01
#define  SSD1306_SWITCHCAPVCC                            0x02
#define  SSD1306_SETHIGHCOLUMN                           0x10
#define  SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
#define  SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
#define  SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define  SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A
#define  SSD1306_DEACTIVATE_SCROLL                       0x2E
#define  SSD1306_ACTIVATE_SCROLL                         0x2F
#define  SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
#define  SSD1306_COMSCANINC                              0xC0
*/


// _SSD1306_HEADER_
#endif
