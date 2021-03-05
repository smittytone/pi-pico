#include "main.h"


/*
 * I2C Functions
 *
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
 *
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

void ssd1306_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t colour, bool fill) {

    if (x + width > oled_width) width = oled_width - x;
    if (y + height > oled_height) height = oled_height - y;

    if (colour != 0 && colour != 1) colour = 1;

    for (uint8_t i = y ; i < y + height ; ++i) {
        for (uint8_t j = x ; j < x + width ; ++j) {
            if (i == y || i == y + height - 1 || j == x || j == x + width - 1) ssd1306_plot(j, i, colour);
        }
    }
}

void ssd1306_text(uint8_t x, uint8_t y, char *the_string, bool do_wrap, bool do_double) {

    uint8_t space_size = do_double ? 4 : 1;
    uint8_t bit_max = do_double ? 16 : 8;

    for (size_t i = 0; i < strlen(the_string); ++i) {
        uint8_t asc_val = the_string[i] - 32;
        uint8_t glyph_len = strlen(CHARSET[asc_val]);

        char glyph[6];
        for (uint j = 0 ; j < 6 ; ++j) {
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

        for (uint8_t j = 1 ; j < sizeof(glyph) + 1 ; ++j) {
            if (j == sizeof(glyph)) {
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