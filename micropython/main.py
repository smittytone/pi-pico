# IMPORTS
import utime as time
from machine import I2C, Pin
from ht16k33matrix import HT16K33Matrix

# CONSTANTS
DELAY = 0.01
PAUSE = 3

# START
if __name__ == '__main__':
    i2c = I2C(0, scl=Pin(9), sda=Pin(8))
    display = HT16K33Matrix(i2c)
    display.set_brightness(2)

    # Draw a custom icon on the LED
    icon = b"\x3C\x42\xA9\x85\x85\xA9\x42\x3C"
    display.set_icon(icon).draw()
    time.sleep(PAUSE)

    # Rotate the icon
    display.set_angle(900).draw()
    time.sleep(PAUSE)

    # Clear the LED
    display.clear().draw()

    # Record two custom icons using 'define_character()'
    icon = b"\x0E\x18\xBE\x6D\x3D\x3C"
    display.define_character(icon, 0)
    icon = b"\x3C\x3D\x6D\xBE\x18\x0E"
    display.define_character(icon, 1)

    # Display scrolling text
    text = "    abcdefghijklmnopqrstuvwxyz 0123456789!$%&*() \x00\x01    "
    display.scroll_text(text)
    time.sleep(PAUSE)

    # Show the previously stored custom icon then Blink the LED
    display.set_character(0, True).draw()
    display.set_blink_rate(1)
    time.sleep(PAUSE)

    # Inverse the pixes
    display.set_inverse().draw()
    time.sleep(PAUSE)

    # Inverse the pixels (to revert)
    display.set_inverse().draw()
    time.sleep(PAUSE)

    # Clear and stop blinking
    display.clear().draw()
    display.set_blink_rate(0)

    # Plot an X
    for i in range(4):
        display.plot(i, i).plot(7 - i, i).plot(i, 7 - i).plot(7 - i, 7 - i)
    display.draw()
    time.sleep(PAUSE)
    assert (display.is_set(0, 0) is True) and (display.is_set(0, 1) is False)
    display.clear().draw()

    # Show an animation
    while True:
        x = 7
        y = 0;
        dx = 0
        dy = 1;
        mx = 6
        my = 7;
        nx = 0
        ny = 0;

        for i in range(0,64):
            display.plot(x, y).draw();

            if dx == 1 and x == mx:
                dy = 1;
                dx = 0;
                mx -= 1;
            elif dx == -1 and x == nx:
                nx += 1;
                dy = -1;
                dx = 0;
            elif dy == 1 and y == my:
                dy = 0;
                dx = -1;
                my -= 1;
            elif dy == -1 and y == ny:
                dx = 1;
                dy = 0;
                ny += 1;

            x += dx;
            y += dy

            time.sleep(DELAY)

        x = 4
        y = 3
        dx = -1
        dy = 0
        mx = 5
        my = 4
        nx = 3
        ny = 2

        for i in range(0, 64):
            display.plot(x, y, 0).draw()

            if dx == 1 and x == mx:
                dy = -1;
                dx = 0;
                mx += 1;
            elif dx == -1 and x == nx:
                nx -= 1;
                dy = 1;
                dx = 0;
            elif dy == 1 and y == my:
                dy = 0;
                dx = 1;
                my += 1;
            elif dy == -1 and y == ny:
                dx = -1;
                dy = 0;
                ny -= 1;

            x += dx;
            y += dy

            time.sleep(DELAY)
