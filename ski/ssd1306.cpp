/*
 * ski for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "ski.h"


/*
 * Ascii Text Character Set
 */
const char *CHARSET[] = {
    "\x02\x00\x00",              // space - Ascii 32
    "\x01\xFA",                  // !
    "\x03\xc0\x00\xc0",          // "
    "\x05\x24\x7e\x24\x7e\x24",  // #
    "\x04\x24\xd4\x56\x48",      // $
    "\x05\xc6\xc8\x10\x26\xc6",  // %
    "\x05\x6c\x92\x6a\x04\x0a",  // &
    "\x01\xc0",                  // '
    "\x02\x7c\x82",              // (
    "\x02\x82\x7c",              // )
    "\x05\x10\x7c\x38\x7c\x10",  // *
    "\x05\x10\x10\x7c\x10\x10",  // +
    "\x02\x06\x07",              // ,
    "\x04\x10\x10\x10\x10",      // -
    "\x02\x06\x06",              // .
    "\x05\x04\x08\x10\x20\x40",  // /
    "\x05\x7C\x8A\x92\xA2\x7C",  // 0 - Ascii 48
    "\x03\x42\xFE\x02",          // 1
    "\x05\x46\x8A\x92\x92\x62",  // 2
    "\x05\x44\x92\x92\x92\x6C",  // 3
    "\x05\x18\x28\x48\xFE\x08",  // 4
    "\x05\xF4\x92\x92\x92\x8C",  // 5
    "\x05\x3C\x52\x92\x92\x8C",  // 6
    "\x05\x80\x8E\x90\xA0\xC0",  // 7
    "\x05\x6C\x92\x92\x92\x6C",  // 8
    "\x05\x60\x92\x92\x94\x78",  // 9
    "\x02\x36\x36",              // : - Ascii 58
    "\x02\x36\x37",              //
    "\x04\x10\x28\x44\x82",      // <
    "\x05\x24\x24\x24\x24\x24",  // =
    "\x04\x82\x44\x28\x10",      // >
    "\x05\x60\x80\x9a\x90\x60",  // ?
    "\x05\x7c\x82\xba\xaa\x78",  // @
    "\x05\x7e\x90\x90\x90\x7e",  // A - Ascii 65
    "\x05\xfe\x92\x92\x92\x6c",  // B
    "\x05\x7c\x82\x82\x82\x44",  // C
    "\x05\xfe\x82\x82\x82\x7c",  // D
    "\x05\xfe\x92\x92\x92\x82",  // E
    "\x05\xfe\x90\x90\x90\x80",  // F
    "\x05\x7c\x82\x92\x92\x5c",  // G
    "\x05\xfe\x10\x10\x10\xfe",  // H
    "\x03\x82\xfe\x82",          // I
    "\x05\x0c\x02\x02\x02\xfc",  // J
    "\x05\xfe\x10\x28\x44\x82",  // K
    "\x04\xfe\x02\x02\x02",      // L
    "\x05\xfe\x40\x20\x40\xfe",  // M
    "\x05\xfe\x40\x20\x10\xfe",  // N
    "\x05\x7c\x82\x82\x82\x7c",  // O
    "\x05\xfe\x90\x90\x90\x60",  // P
    "\x05\x7c\x82\x92\x8c\x7a",  // Q
    "\x05\xfe\x90\x90\x98\x66",  // R
    "\x05\x64\x92\x92\x92\x4c",  // S
    "\x05\x80\x80\xfe\x80\x80",  // T
    "\x05\xfc\x02\x02\x02\xfc",  // U
    "\x05\xf8\x04\x02\x04\xf8",  // V
    "\x05\xfc\x02\x3c\x02\xfc",  // W
    "\x05\xc6\x28\x10\x28\xc6",  // X
    "\x05\xe0\x10\x0e\x10\xe0",  // Y
    "\x05\x86\x8a\x92\xa2\xc2",  // Z - Ascii 90
    "\x03\xfe\x82\x82",          // [
    "\x05\x40\x20\x10\x08\x04",  // forward slash
    "\x03\x82\x82\xfe",          // ]
    "\x05\x20\x40\x80\x40\x20",  // ^
    "\x05\x02\x02\x02\x02\x02",  // _
    "\x02\xc0\xe0",              // '
    "\x04\x04\x2a\x2a\x1e",      // a - Ascii 97
    "\x04\xfe\x22\x22\x1c",      // b
    "\x04\x1c\x22\x22\x22",      // c
    "\x04\x1c\x22\x22\xfc",      // d
    "\x04\x1c\x2a\x2a\x10",      // e
    "\x04\x10\x7e\x90\x80",      // f
    "\x04\x18\x25\x25\x3e",      // g
    "\x04\xfe\x20\x20\x1e",      // h
    "\x02\xbc\x02",              // i
    "\x04\x02\x01\x21\xbe",      // j
    "\x04\xfe\x08\x14\x22",      // k
    "\x02\xfc\x02",              // l
    "\x05\x3e\x20\x18\x20\x1e",  // m
    "\x04\x3e\x20\x20\x1e",     // n
    "\x04\x1c\x22\x22\x1c",      // o
    "\x04\x3f\x22\x22\x1c",      // p
    "\x04\x1c\x22\x22\x3f",      // q
    "\x04\x22\x1e\x20\x10",      // r
    "\x04\x12\x2a\x2a\x04",      // s
    "\x04\x20\x7c\x22\x04",      // t
    "\x04\x3c\x02\x02\x3e",      // u
    "\x05\x38\x04\x02\x04\x38",  // v
    "\x05\x3c\x06\x0c\x06\x3c",  // w
    "\x05\x22\x14\x08\x14\x22",  // x
    "\x04\x39\x05\x06\x3c",      // y
    "\x04\x26\x2a\x2a\x32",      // z - Ascii 122
    "\x04\x10\x7c\x82\x82",      //
    "\x01\xee",                  // |
    "\x04\x82\x82\x7c\x10",      //
    "\x04\x40\x80\x40\x80",      // ~
    "\x04\x60\x90\x90\x60"       // Degrees sign - Ascii 127
};


SSD1306::SSD1306(uint8_t pixel_width, uint8_t pixel_height, bool extra_row = false) {
    is_inverted = false;
    has_hidden_row = extra_row;
    screen_width  = pixel_width;
    screen_height = pixel_height;
    buffer_size = ((pixel_width * pixel_height) >> 3);
    i2c_buffer_size = buffer_size + 1;
    if (has_hidden_row) buffer_size += 128;

    // Set the I2C address based on size
    i2c_address = pixel_height > 32 ? SSD1306_ADDRESS_128x64 : SSD1306_ADDRESS_128x32;

    // Initialise the I2C TX buffer
    for (uint16_t i = 0 ; i < i2c_buffer_size; ++i) {
        i2c_buffer[i] = 0x00;
    }

    // Set up I2C and the display
    setup();
}


void SSD1306::plot(int8_t x, int8_t y, uint8_t colour) {
    // Plot a pixel of the specified colour (1 or 0) at (x,y)
    // Get the buffer byte holding the pixel
    uint16_t byte_index = coords_to_index(x, y);
    uint8_t value = buffer[byte_index];

    if (!has_hidden_row) {
        if ((x > screen_width - 1) || (y > screen_height - 1)) return;
    }
    // Get the specific bit representing the pixel
    uint8_t bit = y - ((y >> 3) << 3);

    if (colour == 1) {
        // Set the pixel
        value |= (1 << bit);
    } else {
        // Clear the pixel
        value &= ~(1 << bit);
    }

    // Write the altered byte back to the buffer
    buffer[byte_index] = value;
}


void SSD1306::rect(int8_t x, int8_t y, uint8_t width, uint8_t height, uint8_t colour, bool fill) {
    // Draw a rectangle with top left at (x,y) and of the specified width and height
    // (including the 1px border if 'fill' is false. Colour is 1 or 0
    if (!has_hidden_row) {
        if (x + width > screen_width) width = screen_width - x;
        if (y + height > screen_height) height = screen_height - y;
    }

    if (colour != 0 && colour != 1) colour = 1;

    for (uint8_t i = y ; i < y + height ; ++i) {
        for (uint8_t j = x ; j < x + width ; ++j) {
            if (!fill && j > x && j < x + width - 1 && i > y && i < y + height - 1) continue;
            plot(j, i, colour);
        }
    }
}


void SSD1306::line(int8_t x, int8_t y, int8_t tox, int8_t toy, uint8_t colour, uint8_t thick) {
    // Draw a line of thickness 'thick' between (x,y) and (tox,toy)
    // Colour is 1 or 0
    if (thick < 1) thick = 1;

    // Look for vertical and horizontal lines
    bool track_by_x = true;
    if (x == tox) track_by_x = false;
    if (y == toy && !track_by_x) return;

    uint8_t start = 0;
    uint8_t end = 0;
    float m = 1;

    // Swap start and end values for L-R raster
    if (track_by_x) {
        if (x > tox) {
            uint8_t a = x;
            x = tox;
            tox = a;
        }
        start = x;
        end = tox;
        m = (float)(toy - y) / (float)(tox - x);
    } else {
        if (y > toy) {
            uint8_t a = y;
            y = toy;
            toy = a;
        }
        start = y;
        end = toy;
        m = (float)(tox - x) / (float)(toy - y);
    }

    // Run for 'thick' times to generate thickness
    for (uint8_t j = 0 ; j < thick ; ++j) {
        // Run from x to tox, calculating the y offset at each point
        for (uint8_t i = start ; i < end ; ++i) {
            if (track_by_x) {
                uint8_t dy = y + (uint8_t)(m * (i - x)) + j;
                plot(i, dy, colour);
            } else {
                uint8_t dx = x + (uint8_t)(m * (i - y)) + j;
                plot(dx, i, colour);
            }
        }
    }
}


void SSD1306::text(int8_t x, int8_t y, string the_string, bool do_wrap, bool do_double) {
    // Print the supplied string at (x,y) (top-left co-ordinate), wrapping to the next line
    // if required. 'do_double' selects double-height output (currently not working)
    uint8_t space_size = do_double ? 4 : 1;
    uint8_t bit_max = do_double ? 16 : 8;

    for (size_t i = 0 ; i < the_string.length() ; ++i) {
        char glyph[6];
        char col_1 = 0;
        char col_0 = 0;
        ssize_t glyph_len = 0;

        uint8_t asc_val = the_string[i] - 32;
        glyph_len = CHARSET[asc_val][0] + 1;
        for (size_t j = 0 ; j < glyph_len ; ++j) {
            glyph[j] = (j == glyph_len - 1 ? 0x00 : CHARSET[asc_val][j + 1]);
        }

        col_0 = text_flip(glyph[0]);

        if (do_wrap) {
            if ((x + glyph_len * (do_double ? 2 : 1) >= screen_width)) {
                if (y + bit_max < screen_height) {
                    x = 0;
                    y += bit_max;
                } else {
                    return;
                }
            }
        }

        for (size_t j = 1 ; j < glyph_len + 1 ; ++j) {
            if (j == glyph_len) {
                //if (do_double) break;
                col_1 = glyph[j - 1];
            } else{
                col_1 = text_flip(glyph[j]);
            }

            uint16_t col_0_right = 0;
            uint16_t col_1_right = 0;
            uint16_t col_0_left = 0;
            uint16_t col_1_left = 0;

            if (do_double) {
                col_0_right = text_stretch(col_0);
                col_0_left = col_0_right;
                col_1_right = text_stretch(col_1);
                col_1_left = col_1_right;

                for (int8_t a = 6 ; a >= 0 ; --a) {
                    for (uint8_t b = 1 ; b < 3 ; b++) {
                        if ((((col_0 >> a) & 3) == 3 - b) && (((col_1 >> a) & 3) == b)) {
                            col_0_right |= (1 << ((a * 2) + b));
                            col_1_left |= (1 << ((a * 2) + 3 - b));
                        }
                    }
                }
            }

            uint8_t z = (y - ((y >> 3) << 3)) - 1;

            for (uint8_t k = 0 ; k < bit_max ; ++k) {
                if (((y + k) % 8) == 0 && k > 0) {
                    z = 0;
                } else {
                    z += 1;
                }

                if (do_double) {
                    if (x < screen_width)     plot_char(x,     y, k, col_0_left,  z);
                    if (x + 1 < screen_width) plot_char(x + 1, y, k, col_0_right, z);
                    if (x + 2 < screen_width) plot_char(x + 2, y, k, col_1_left,  z);
                    if (x + 3 < screen_width) plot_char(x + 3, y, k, col_1_right, z);
                } else {
                    if (x < screen_width) plot_char(x, y, k, col_0, z);
                }
            }

            x += (do_double ? 2 : 1);

            if (x >= screen_width) {
                if (!do_wrap) return;
                if (y + bit_max < screen_height) {
                    x = 0;
                    y += bit_max;
                } else {
                    break;
                }
            }

            col_0 = col_1;
        }
    }
}


void SSD1306::scroll() {

    uint8_t max_y = has_hidden_row ? screen_height : screen_height - 8;
    for (uint8_t y = 4 ; y < max_y ; y += 4) {
        bool is_upper = (y % 8 != 0);
        for (uint8_t x = 0 ; x < 128 ; ++x) {
            uint16_t src = coords_to_index(x, y);
            uint16_t dst = coords_to_index(x, y - 4);
            uint8_t nibble = buffer[src] & (is_upper ? 0xF0 : 0x0F);

            if (is_upper) {
                buffer[dst] = (nibble >> 4);
            } else {
                buffer[dst] += nibble;
            }
        }
    }
}


void SSD1306::bit_scroll() {

    uint8_t max_y = has_hidden_row ? screen_height : screen_height - 8;
    for (uint8_t y = 0 ; y <= max_y ; y += 8) {
        for (uint8_t x = 0 ; x < 128 ; ++x) {
            uint16_t current_byte = coords_to_index(x, y);
            uint8_t v = buffer[current_byte];
            v = (v >> 1);

            if (y < max_y) {
                uint16_t next_byte = coords_to_index(x, y + 8);
                uint8_t v2 = buffer[next_byte];
                v |= ((v2 & 0x01) << 7);
            }

            buffer[current_byte] = v;
        }
    }
}


void SSD1306::inverse(bool do_invert) {
    // Invert the display using SSD1306 setting
    uint8_t data = do_invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY;
    is_inverted = do_invert;
    write(true, &data, 1);
}


void SSD1306::clear() {
    for (uint16_t i = 0 ; i < buffer_size ; ++i) {
        buffer[i] = 0x00;
    }
}


void SSD1306::draw() {
    // Write the buffer out to the display
    write(false, &buffer[0], buffer_size);
}


void SSD1306::setup() {
    // Set up the display

    // Set up I2C
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    // Initialize the display
    uint8_t data[3] = {SSD1306_DISPLAYOFF, SSD1306_DISPLAYOFF, SSD1306_DISPLAYOFF};
    write(true, data, 1);

    data[0] = SSD1306_SETDISPLAYCLOCKDIV;
    data[1] = 0x80;
    write(true, data, 2);

    data[0] = SSD1306_SETMULTIPLEX;
    data[1] = screen_height - 1;
    write(true, data, 2);

    data[0] = SSD1306_SETDISPLAYOFFSET;
    data[1] = 0x00;
    write(true, data, 2);

    data[0] = SSD1306_SETSTARTLINE;
    write(true, data, 1);

    data[0] = SSD1306_CHARGEPUMP;
    data[1] = 0x14;
    write(true, data, 2);

    data[0] = SSD1306_MEMORYMODE;
    data[1] = 0x00;
    write(true, data, 2);

    data[0] = SSD1306_SEGREMAP;
    write(true, data, 1);

    data[0] = SSD1306_COMSCANDEC;
    write(true, data, 1);

    data[0] = SSD1306_SETCOMPINS;
    data[1] = (screen_height == 32 || screen_height == 16 ? 0x02 : 0x12);
    write(true, data, 2);

    data[0] = SSD1306_SETCONTRAST;
    data[1] = 0x8F;
    write(true, data, 2);

    data[0] = SSD1306_SETPRECHARGE;
    data[1] = 0xF1;
    write(true, data, 2);

    data[0] = SSD1306_SETVCOMDETECT;
    data[1] = 0x40;
    write(true, data, 2);

    data[0] = SSD1306_DISPLAYALLON_RESUME;
    write(true, data, 1);

    data[0] = SSD1306_NORMALDISPLAY;
    write(true, data, 1);

    data[0] = SSD1306_DISPLAYON;
    write(true, data, 1);

    data[0] = SSD1306_COLUMNADDR;
    data[1] = 0x00;
    data[2] = screen_width - 1;
    write(true, data, 3);

    data[0] = SSD1306_PAGEADDR;
    data[1] = 0x00;
    data[2] = (screen_height >> 3) - 1;
    write(true, data, 3);

    // Clear the OLED
    clear();
    draw();
}


uint16_t SSD1306::coords_to_index(uint8_t x, uint8_t y) {
    // Convert pixel co-ordinates to a bytearray index
    return ((y >> 3) * screen_width) + x;
}


void SSD1306::write(bool is_command, uint8_t *data, uint16_t data_size) {
    // Write a command or data to the display via I2C
    i2c_buffer[0] = (is_command ? SSD1306_DO_CMD : SSD1306_WRITE_TO_BUFFER);

    // Copy the input data to index 1 of the TX array
    if (data_size <= i2c_buffer_size - 1) {
        memcpy(&i2c_buffer[1], data, data_size);
        i2c_write_block(i2c_buffer, data_size + 1);
    } else {
        // Copy the just first 1024 bytes
        memcpy(&i2c_buffer[1], data, i2c_buffer_size - 1);
        i2c_write_block(i2c_buffer, i2c_buffer_size);
    }
}


void SSD1306::i2c_write_byte(uint8_t byte) {
    // Convenience function to write a single byte to the matrix
    i2c_write_blocking(I2C_PORT, i2c_address, &byte, 1, false);
}


void SSD1306::i2c_write_block(uint8_t *data, uint16_t count) {
    // Convenience function to write 'count' bytes to the matrix
    uint16_t r = i2c_write_blocking(I2C_PORT, i2c_address, data, count, false);
    if (r != count) {
        std::cout << "I2C error" << std::endl;
    }
}


uint8_t SSD1306::text_flip(uint8_t value) {
    // Rotates the character array from the saved state
    // to that required by the screen orientation
    uint8_t flipped = 0;
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        if ((value & (1 << i)) > 0) flipped += (1 << (7 - i));
    }
    return flipped;
}


uint16_t SSD1306::text_stretch(uint8_t x) {
    // Pixel-doubles an 8-bit value to 16 bits
    uint16_t v = (x & 0xF0) << 4 | (x & 0x0F);
    v = (v << 2 | v) & 0x3333;
    v = (v << 1 | v) & 0x5555;
    v |= (v << 1);
    return v;
}


void SSD1306::plot_char(int8_t x, int8_t y, uint8_t char_bit, uint16_t char_byte, uint8_t byte_bit) {
    // Write a line from a character glyph to the buffer
    if (x < 0 || x >= screen_width) return;
    if (y + char_bit < 0 || y + char_bit >= screen_height) return;
    uint16_t byte = coords_to_index(x, y + char_bit);
    if ((char_byte & (1 << char_bit)) != 0) buffer[byte]  |= (1 << byte_bit);
}