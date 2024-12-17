/*
 * Phantoms
 *
 * @version     1.0.1
 * @author      smittytone
 * @copyright   2024, Tony Smith
 * @licence     MIT
 *
 */
#include "main.h"


extern uint8_t oled_height;
extern uint8_t oled_width;
extern uint8_t oled_i2c_addr;
extern bool oled_inverted;
extern uint8_t oled_buffer[1024];
extern uint8_t temp_buffer[1024];
extern uint8_t side_buffer[1024];
extern uint8_t i2c_tx_buffer[1025];
extern uint16_t oled_buffer_size;
extern uint16_t i2c_tx_buffer_size;
extern uint8_t *draw_buffer;

/*
 * Ascii Text Character Set
 */
const char CHARSET[128][6] = {
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


/*
 * Trig. tables, because it's easier to lookup than calculate
 */
float COS_TABLE[] = {
    0.000,0.035,0.070,0.105,0.140,0.174,0.208,0.243,0.276,0.310,0.343,0.376,0.408,0.439,0.471,0.501,0.531,0.561,0.589,0.617,0.644,
    0.671,0.696,0.721,0.745,0.768,0.790,0.810,0.830,0.849,0.867,0.884,0.900,0.915,0.928,0.941,0.952,0.962,0.971,0.979,0.985,0.991,
    0.995,0.998,1.000,1.000,0.999,0.997,0.994,0.990,0.984,0.977,0.969,0.960,0.949,0.938,0.925,0.911,0.896,0.880,0.863,0.845,0.826,
    0.806,0.784,0.762,0.739,0.715,0.690,0.664,0.638,0.610,0.582,0.554,0.524,0.494,0.463,0.432,0.400,0.368,0.335,0.302,0.268,0.234,
    0.200,0.166,0.131,0.096,0.062,0.027,-0.008,-0.043,-0.078,-0.113,-0.148,-0.182,-0.217,-0.251,-0.284,-0.318,-0.351,-0.383,-0.415,
    -0.447,-0.478,-0.508,-0.538,-0.567,-0.596,-0.624,-0.651,-0.677,-0.702,-0.727,-0.750,-0.773,-0.795,-0.815,-0.835,-0.854,-0.872,
    -0.888,-0.904,-0.918,-0.931,-0.944,-0.955,-0.964,-0.973,-0.981,-0.987,-0.992,-0.996,-0.998,-1.000,-1.000,-0.999,-0.997,-0.993,
    -0.988,-0.982,-0.975,-0.967,-0.957,-0.947,-0.935,-0.922,-0.908,-0.893,-0.876,-0.859,-0.840,-0.821,-0.801,-0.779,-0.757,-0.733,
    -0.709,-0.684,-0.658,-0.631,-0.604,-0.575,-0.547,-0.517,-0.487,-0.456,-0.424,-0.392,-0.360,-0.327,-0.294,-0.260,-0.226,-0.192,
    -0.158,-0.123,-0.088,-0.053,-0.018};

float SIN_TABLE[] = {
    1.000,0.999,0.998,0.994,0.990,0.985,0.978,0.970,0.961,0.951,0.939,0.927,0.913,0.898,0.882,0.865,0.847,0.828,0.808,0.787,
    0.765,0.742,0.718,0.693,0.667,0.641,0.614,0.586,0.557,0.528,0.498,0.467,0.436,0.404,0.372,0.339,0.306,0.272,0.238,0.204,
    0.170,0.135,0.101,0.066,0.031,-0.004,-0.039,-0.074,-0.109,-0.144,-0.178,-0.213,-0.247,-0.280,-0.314,-0.347,-0.379,-0.412,
    -0.443,-0.474,-0.505,-0.535,-0.564,-0.593,-0.620,-0.647,-0.674,-0.699,-0.724,-0.747,-0.770,-0.792,-0.813,-0.833,-0.852,
    -0.870,-0.886,-0.902,-0.916,-0.930,-0.942,-0.953,-0.963,-0.972,-0.980,-0.986,-0.991,-0.995,-0.998,-1.000,-1.000,-0.999,
    -0.997,-0.994,-0.989,-0.983,-0.976,-0.968,-0.959,-0.948,-0.936,-0.924,-0.910,-0.895,-0.878,-0.861,-0.843,-0.823,-0.803,
    -0.782,-0.759,-0.736,-0.712,-0.687,-0.661,-0.635,-0.607,-0.579,-0.550,-0.520,-0.490,-0.459,-0.428,-0.396,-0.364,-0.331,
    -0.298,-0.264,-0.230,-0.196,-0.162,-0.127,-0.092,-0.057,-0.022,0.013,0.048,0.083,0.117,0.152,0.187,0.221,0.255,0.288,
    0.322,0.355,0.387,0.419,0.451,0.482,0.512,0.542,0.571,0.599,0.627,0.654,0.680,0.705,0.730,0.753,0.776,0.797,0.818,0.837,
    0.856,0.874,0.890,0.906,0.920,0.933,0.945,0.956,0.966,0.974,0.981,0.988,0.992,0.996,0.999,1.000};


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

    // Initialise the I2C TX buffer
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
    // Plot a pixel of the specified colour (1 or 0) at (x,y)
    // Get the buffer byte holding the pixel
    uint16_t byte_index = ssd1306_coords_to_index(x, y);
    uint8_t value = draw_buffer[byte_index];

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
    draw_buffer[byte_index] = value;
}


void ssd1306_line(uint8_t x, uint8_t y, uint8_t tox, uint8_t toy, uint8_t colour, uint8_t thick) {
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
    // (including the 1px border if 'fill' is false. Colour is 1 or 0
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


void ssd1306_circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t colour, bool fill) {
    // Draw a circle with its centre on the specified co-ordinates and of the
    // required radius
    if (colour != 0 && colour != 1) colour = 1;
    for (uint8_t i = 0 ; i < 180 ; ++i) {
        int8_t a = x - (int8_t)((float)radius * SIN_TABLE[i]);
        int8_t b = y - (int8_t)((float)radius * COS_TABLE[i]);

        if (a >= 0 && a < oled_width && b >= 0 && b < oled_height) {
            ssd1306_plot(a, b, colour);

            if (fill) {
                if (a > x) {
                    uint8_t j = x;
                    while(1) {
                        ssd1306_plot(j, b, colour);
                        ++j;
                        if (j >= a) break;
                    }
                } else {
                    uint8_t j = a + 1;
                    while(1) {
                        ssd1306_plot(j, b, colour);
                        ++j;
                        if (j > x) break;
                    }
                }
            }
        }
    }
}


void ssd1306_text(int8_t x, int8_t y, const char *the_string, bool do_wrap, bool do_double) {
    // Print the supplied string at (x,y) (top-left co-ordinate), wrapping to the next line
    // if required. 'do_double' selects double-height output (currently not working)
    uint8_t space_size = do_double ? 4 : 1;
    uint8_t bit_max = do_double ? 16 : 8;

    for (size_t i = 0 ; i < strlen(the_string) ; ++i) {
        char glyph[6];
        char col_1 = 0;
        char col_0 = 0;
        ssize_t glyph_len = 0;

        uint8_t asc_val = the_string[i] - 32;
        glyph_len = CHARSET[asc_val][0] + 1;
        for (size_t j = 0 ; j < glyph_len ; ++j) {
            if (j == glyph_len - 1) {
                glyph[j] = 0x00;
            } else {
                glyph[j] = CHARSET[asc_val][j + 1];
            }
        }

        col_0 = ssd1306_text_flip(glyph[0]);

        if (do_wrap) {
            if ((x + glyph_len * (do_double ? 2 : 1) >= oled_width)) {
                if (y + bit_max < oled_height) {
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
                col_1 = ssd1306_text_flip(glyph[j]);
            }

            uint16_t col_0_right = 0;
            uint16_t col_1_right = 0;
            uint16_t col_0_left = 0;
            uint16_t col_1_left = 0;

            if (do_double) {
                col_0_right = ssd1306_text_stretch(col_0);
                col_0_left = col_0_right;
                col_1_right = ssd1306_text_stretch(col_1);
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
                    if (x < oled_width)     ssd1306_char_plot(x,     y, k, col_0_left,  z);
                    if (x + 1 < oled_width) ssd1306_char_plot(x + 1, y, k, col_0_right, z);
                    if (x + 2 < oled_width) ssd1306_char_plot(x + 2, y, k, col_1_left,  z);
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
    }
}


void ssd1306_clear() {
    // Clear the buffer
    // NOTE Array's first byte is the command indicator
    for (uint16_t i = 0 ; i < oled_buffer_size; i++) {
        draw_buffer[i] = 0x00;
    }
}


void ssd1306_draw() {
    // Write the buffer out to the display
    ssd1306_write(false, draw_buffer, oled_buffer_size);
}


void ssd1306_write(bool is_command, uint8_t *data, uint16_t data_size) {
    // Write a command or data to the display via I2C
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


uint16_t ssd1306_text_stretch(uint8_t x) {
    // Pixel-doubles an 8-bit value to 16 bits
    uint16_t v = (x & 0xF0) << 4 | (x & 0x0F);
    v = (v << 2 | v) & 0x3333;
    v = (v << 1 | v) & 0x5555;
    v |= (v << 1);
    return v;
}


void ssd1306_char_plot(int8_t x, int8_t y, uint8_t char_bit, uint16_t char_byte, uint8_t byte_bit) {
    // Write a line from a character glyph to the buffer
    if (x < 0 || x >= oled_width) return;
    if (y + char_bit < 0 || y + char_bit >= oled_height) return;
    uint16_t byte = ssd1306_coords_to_index(x, y + char_bit);
    if ((char_byte & (1 << char_bit)) != 0) draw_buffer[byte]  |= (1 << byte_bit);
}
