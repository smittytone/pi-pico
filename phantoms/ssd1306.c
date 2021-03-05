#include "main.h"


/*
 * Ascii Text Character Set
 */
const char CHARSET[128][6] = {
    "\x00\x00\x00",              // space - Ascii 32
    "\xfa\x00",                  // !
    "\xc0\x00\xc0\x00",          // "
    "\x24\x7e\x24\x7e\x24\x00",  // #
    "\x24\xd4\x56\x48\x00",      // $
    "\xc6\xc8\x10\x26\xc6\x00",  // %
    "\x6c\x92\x6a\x04\x0a\x00",  // &
    "\xc0\x00",                  // '
    "\x7c\x82\x00",              // (
    "\x82\x7c\x00",              // )
    "\x10\x7c\x38\x7c\x10\x00",  // *
    "\x10\x10\x7c\x10\x10\x00",  // +
    "\x06\x07\x00",              // ,
    "\x10\x10\x10\x10\x00",      // -
    "\x06\x06\x00",              // .
    "\x04\x08\x10\x20\x40\x00",  // /
    "\x7c\x8a\x92\xa2\x7c\x00",  // 0 - Ascii 48
    "\x42\xfe\x02\x00",          // 1
    "\x46\x8a\x92\x92\x62\x00",  // 2
    "\x44\x92\x92\x92\x6c\x00",  // 3
    "\x18\x28\x48\xfe\x08\x00",  // 4
    "\xf4\x92\x92\x92\x8c\x00",  // 5
    "\x3c\x52\x92\x92\x8c\x00",  // 6
    "\x80\x8e\x90\xa0\xc0\x00",  // 7
    "\x6c\x92\x92\x92\x6c\x00",  // 8
    "\x60\x92\x92\x94\x78\x00",  // 9
    "\x36\x36\x00",              // : - Ascii 58
    "\x36\x37\x00",              //
    "\x10\x28\x44\x82\x00",      // <
    "\x24\x24\x24\x24\x24\x00",  // =
    "\x82\x44\x28\x10\x00",      // >
    "\x60\x80\x9a\x90\x60\x00",  // ?
    "\x7c\x82\xba\xaa\x78\x00",  // @
    "\x7e\x90\x90\x90\x7e\x00",  // A - Ascii 65
    "\xfe\x92\x92\x92\x6c\x00",  // B
    "\x7c\x82\x82\x82\x44\x00",  // C
    "\xfe\x82\x82\x82\x7c\x00",  // D
    "\xfe\x92\x92\x92\x82\x00",  // E
    "\xfe\x90\x90\x90\x80\x00",  // F
    "\x7c\x82\x92\x92\x5c\x00",  // G
    "\xfe\x10\x10\x10\xfe\x00",  // H
    "\x82\xfe\x82\x00",          // I
    "\x0c\x02\x02\x02\xfc\x00",  // J
    "\xfe\x10\x28\x44\x82\x00",  // K
    "\xfe\x02\x02\x02\x00",      // L
    "\xfe\x40\x20\x40\xfe\x00",  // M
    "\xfe\x40\x20\x10\xfe\x00",  // N
    "\x7c\x82\x82\x82\x7c\x00",  // O
    "\xfe\x90\x90\x90\x60\x00",  // P
    "\x7c\x82\x92\x8c\x7a\x00",  // Q
    "\xfe\x90\x90\x98\x66\x00",  // R
    "\x64\x92\x92\x92\x4c\x00",  // S
    "\x80\x80\xfe\x80\x80\x00",  // T
    "\xfc\x02\x02\x02\xfc\x00",  // U
    "\xf8\x04\x02\x04\xf8\x00",  // V
    "\xfc\x02\x3c\x02\xfc\x00",  // W
    "\xc6\x28\x10\x28\xc6\x00",  // X
    "\xe0\x10\x0e\x10\xe0\x00",  // Y
    "\x86\x8a\x92\xa2\xc2\x00",  // Z - Ascii 90
    "\xfe\x82\x82\x00",          // [
    "\x40\x20\x10\x08\x04\x00",  // forward slash
    "\x82\x82\xfe\x00",          // ]
    "\x20\x40\x80\x40\x20\x00",  // ^
    "\x02\x02\x02\x02\x02\x00",  // _
    "\xc0\xe0\x00",              // '
    "\x04\x2a\x2a\x1e\x00",      // a - Ascii 97
    "\xfe\x22\x22\x1c\x00",      // b
    "\x1c\x22\x22\x22\x00",      // c
    "\x1c\x22\x22\xfc\x00",      // d
    "\x1c\x2a\x2a\x10\x00",      // e
    "\x10\x7e\x90\x80\x00",      // f
    "\x18\x25\x25\x3e\x00",      // g
    "\xfe\x20\x20\x1e\x00",      // h
    "\xbc\x02\x00",              // i
    "\x02\x01\x21\xbe\x00",      // j
    "\xfe\x08\x14\x22\x00",      // k
    "\xfc\x02\x00",              // l
    "\x3e\x20\x18\x20\x1e\x00",  // m
    "\x3e\x20\x20 \x1e\x00",     // n
    "\x1c\x22\x22\x1c\x00",      // o
    "\x3f\x22\x22\x1c\x00",      // p
    "\x1c\x22\x22\x3f\x00",      // q
    "\x22\x1e\x20\x10\x00",      // r
    "\x12\x2a\x2a\x04\x00",      // s
    "\x20\x7c\x22\x04\x00",      // t
    "\x3c\x02\x02\x3e\x00",      // u
    "\x38\x04\x02\x04\x38\x00",  // v
    "\x3c\x06\x0c\x06\x3c\x00",  // w
    "\x22\x14\x08\x14\x22\x00",  // x
    "\x39\x05\x06\x3c\x00",      // y
    "\x26\x2a\x2a\x32\x00",      // z - Ascii 122
    "\x10\x7c\x82\x82\x00",      //
    "\xee\x00",                  // |
    "\x82\x82\x7c\x10\x00",      //
    "\x40\x80\x40\x80\x00",      // ~
    "\x60\x90\x90\x60\x00"       // Degrees sign - Ascii 127
};


/*
 * I2C Functions
 */
void i2c_write_byte(uint8_t byte) {
    // Convenience function to write a single byte to the matrix
    i2c_write_blocking(I2C_PORT, oled_i2c_addr, &byte, 1, false);
}

void i2c_write_block(uint8_t *data, uint16_t count) {
    // Convenience function to write 'count' bytes to the matrix
    uint16_t r = i2c_write_blocking(I2C_PORT, oled_i2c_addr, data, count, false);
    if (r != count) printf("(I2C err: %i", r);
}


/*
 * SSD1306 Functions
 */
void ssd1306_init(uint8_t pixel_width, uint8_t pixel_height) {

    // Set up the screen data
    oled_width = pixel_width;
    oled_height = pixel_height;
    oled_inverted = false;
    oled_buffer_size = ((oled_width * oled_height) >> 3);
    i2c_tx_buffer_size = oled_buffer_size + 1;

    // Set the I2C address based on size
    oled_i2c_addr = oled_height > 32 ? SSD1306_ADDRESS_128x64 : SSD1306_ADDRESS_128x32;

    // Iniialize the I2C TX buffer
    for (uint16_t i = 0 ; i < i2c_tx_buffer_size; i++) {
        i2c_tx_buffer[i] = 0x00;
    }

    // Set up the GPIO RST pin
    gpio_init(SSD1306_RST_PIN);
    gpio_set_dir(SSD1306_RST_PIN, true);

    //  Toggle the RST pin over 1ms + 10ms
    gpio_put(SSD1306_RST_PIN, ON);
    sleep_ms(1);
    gpio_put(SSD1306_RST_PIN, OFF);
    sleep_ms(10);
    gpio_put(SSD1306_RST_PIN, ON);

    // Initialize the display
    uint8_t data[3] = {SSD1306_DISPLAYOFF,SSD1306_DISPLAYOFF,SSD1306_DISPLAYOFF};
    ssd1306_write(true, data, 1);

    data[0] = SSD1306_SETDISPLAYCLOCKDIV;
    data[1] = 0x80;
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_SETMULTIPLEX;
    data[1] = oled_height - 1;
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_SETDISPLAYOFFSET;
    data[1] = 0x00;
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_SETSTARTLINE;
    ssd1306_write(true, data, 1);

    data[0] = SSD1306_CHARGEPUMP;
    data[1] = 0x14;
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_MEMORYMODE;
    data[1] = 0x00;
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_SEGREMAP;
    ssd1306_write(true, data, 1);

    data[0] = SSD1306_COMSCANDEC;
    ssd1306_write(true, data, 1);

    data[0] = SSD1306_SETCOMPINS;
    data[1] = (oled_height == 32 || oled_height == 16 ? 0x02 : 0x12);
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_SETCONTRAST;
    data[1] = 0x8F;
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_SETPRECHARGE;
    data[1] = 0xF1;
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_SETVCOMDETECT;
    data[1] = 0x40;
    ssd1306_write(true, data, 2);

    data[0] = SSD1306_DISPLAYALLON_RESUME;
    ssd1306_write(true, data, 1);

    data[0] = SSD1306_NORMALDISPLAY;
    ssd1306_write(true, data, 1);

    data[0] = SSD1306_DISPLAYON;
    ssd1306_write(true, data, 1);

    data[0] = SSD1306_COLUMNADDR;
    data[1] = 0x00;
    data[2] = oled_width - 1;
    ssd1306_write(true, data, 3);

    data[0] = SSD1306_PAGEADDR;
    data[1] = 0x00;
    data[2] = (oled_height / 8) - 1;
    ssd1306_write(true, data, 3);

    // Clear the OLED
    ssd1306_clear();
    ssd1306_draw();
}

void ssd1306_inverse(bool do_invert) {
    // Invert the display using SSD1306 setting
    uint8_t data = do_invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY;
    oled_inverted = do_invert;
    ssd1306_write(true, &data, 1);
}

void ssd1306_plot(uint8_t x, uint8_t y, uint8_t colour) {

    // Get the buffer byte holding the pixel
    uint16_t byte_index = ssd1306_coords_to_index(x, y);
    uint8_t value = oled_buffer[byte_index];

    if ((x > oled_width - 1) || (y > oled_height - 1)) return;

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
    oled_buffer[byte_index] = value;
}

void ssd1306_line(uint8_t x, uint8_t y, uint8_t tox, uint8_t toy, uint8_t colour, uint8_t thick) {
    // Draw a line of thickness 'thick' between (x,y) and (tox,toy)
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
                if ((i >= 0 && i < oled_width) && (dy >= 0 && dy < oled_height)) ssd1306_plot(i, dy, colour);
            } else {
                uint8_t dx = x + (uint8_t)(m * (i - y)) + j;
                if ((i >= 0 && i < oled_height) && (dx >= 0 && dx < oled_width)) ssd1306_plot(dx, i, colour);
            }
        }
    }
}

void ssd1306_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t colour, bool fill) {
    // Draw a rectangle with top left at (x,y) and of the specified width and height
    // (including the 1px border if 'fill' is false
    if (x + width > oled_width) width = oled_width - x;
    if (y + height > oled_height) height = oled_height - y;
    if (colour != 0 && colour != 1) colour = 1;

    for (uint8_t i = y ; i < y + height ; ++i) {
        for (uint8_t j = x ; j < x + width ; ++j) {
            if (!fill && j > x && j < x + width - 1 && i > y && i < y + height - 1) continue;
            ssd1306_plot(j, i, colour);
        }
    }
}

void ssd1306_text(uint8_t x, uint8_t y, char *the_string, bool do_wrap, bool do_double) {

    uint8_t space_size = do_double ? 4 : 1;
    uint8_t bit_max = do_double ? 16 : 8;

    for (size_t i = 0; i < strlen(the_string); ++i) {
        uint8_t asc_val = the_string[i] - 32;
        uint8_t glyph_len = strlen(CHARSET[asc_val]);

        char glyph[glyph_len];
        for (uint j = 0 ; j < glyph_len ; ++j) {
            if (j < glyph_len) {
                glyph[j] = CHARSET[asc_val][j];
            } else {
                glyph[j] = 0x00;
            }
        }

        uint8_t col_0 = ssd1306_text_flip(glyph[0]);
        uint8_t col_1 = 0;

        if (do_wrap) {
            if (x + sizeof(glyph) * (do_double ? 2 : 1) >= oled_width) {
                if (y + bit_max < oled_height) {
                    x = 0;
                    y += bit_max;
                } else {
                    return;
                }
            }
        }

        for (uint8_t j = 1 ; j < strlen(glyph) + 1 ; ++j) {
            if (j == strlen(glyph)) {
                if (do_double) break;
                col_1 = ssd1306_text_flip(glyph[j - 1]);
            } else{
                col_1 = ssd1306_text_flip(glyph[j]);
            }

            uint8_t col_0_right = 0;
            uint8_t col_1_right = 0;
            uint8_t col_0_left = 0;
            uint8_t col_1_left = 0;

            if (do_double) {
                col_0_right = ssd1306_text_stretch(col_0);
                col_0_left = col_0_right;
                col_1_right = ssd1306_text_stretch(col_1);
                col_1_left = col_1_right;

                for (uint8_t a = 6 ; a >=0 ; --a) {
                    for (uint8_t b = 1 ; b < 3 ; b++) {
                        if ((col_0 >> a & 3 == 3 - b) && (col_1 >> a & 3 == b)) {
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
                    if (x < oled_width) ssd1306_char_plot(x, y, k, col_0_left, z);
                    if (x + 1 < oled_width) ssd1306_char_plot(x + 1, y, k, col_0_right, z);
                    if (x + 2 < oled_width) ssd1306_char_plot(x + 2, y, k, col_1_left, z);
                    if (x + 3 < oled_width) ssd1306_char_plot(x + 3, y, k, col_1_right, z);
                } else {
                    if (x < oled_width) ssd1306_char_plot(x, y, k, col_0, z);
                }
            }

            x += (do_double ? 2 : 1);

            if (x >= oled_width) {
                if (!do_wrap) return;
                if (y + bit_max < oled_height) {
                    x = 0;
                    y += bit_max;
                } else {
                    break;
                }
            }

            col_0 = col_1;
        }

        // Add spacer if we can
        if (i < sizeof(the_string) - 1) {
            x += space_size;
            if (x >= oled_width) {
                if (!do_wrap) return;
                if (y + bit_max < oled_height) {
                    x = 0;
                    y += bit_max;
                } else {
                    break;
                }
            }
        }
    }
}

void ssd1306_clear() {
    // Clear the buffer
    // NOTE Array's first byte is the command indicator
    for (uint16_t i = 0 ; i < oled_buffer_size; i++) {
        oled_buffer[i] = 0x00;
    }
}

void ssd1306_draw() {
    // Write the buffer out to the display
    ssd1306_write(false, oled_buffer, oled_buffer_size);
}

void ssd1306_write(bool is_command, uint8_t *data, uint16_t data_size) {
    // Write a commmand or data to the display via I2C
    i2c_tx_buffer[0] = (is_command ? SSD1306_DO_CMD : SSD1306_WRITE_TO_BUFFER);

    // Copy the input data to index 1 of the TX array
    if (data_size <= i2c_tx_buffer_size - 1) {
        memcpy(&i2c_tx_buffer[1], data, data_size);
    }

    // Write out the TX array
    i2c_write_block(i2c_tx_buffer, data_size + 1);
}

uint16_t ssd1306_coords_to_index(uint8_t x, uint8_t y) {
    // Convert pixel co-ordinates to a bytearray index
    return ((y >> 3) * oled_width) + x;
}

uint8_t ssd1306_text_flip(uint8_t value) {
    // Rotates the character array from the saved state
    // to that required by the screen orientation
    uint8_t flipped = 0;
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        if ((value & (1 << i)) > 0) flipped += (1 << (7 - i));
    }
    return flipped;
}

uint8_t ssd1306_text_stretch(uint8_t x) {
    // Pixel-doubles an 8-bit value to 16 bits
    x = (x & 0xF0) << 4 | (x & 0x0F);
    x = (x << 2 | x) & 0x3333;
    x = (x << 1 | x) & 0x5555;
    x = x | x << 1;
    return x;
}

void ssd1306_char_plot(uint8_t x, uint8_t y, uint8_t k, uint8_t c, uint8_t a) {
    // Write a pixel from a character glyph to the buffer
    uint16_t b = ssd1306_coords_to_index(x, y + k);
    if (c & (1 << k) != 0) oled_buffer[b] |= (1 << a);
}