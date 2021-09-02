# pi-pico

Assorted projects made while playing with the [Raspberry Pi Pico](https://www.raspberrypi.org/documentation/pico/getting-started/).

## Projects

* [Cellular IoT Demo](#cellular-iot-demo-101)
* [Phantom Slayer](#phantom-slayer-102)
* [Hunt the Wumpus](#hunt-the-wumpus-102)
* [Sensor](#sensor)
* [I2C](#i2c)
* [Micropython](#micropython)
* [Makepico](#makepico-210)

**Note** [Makepico](#makepico-210) is a script for creating ready-to-use Pico projects. The other items are Pico demos and games.

---

## Cellular IoT Demo 1.0.1

A sample C++ project bringing the Pico and the [Waveshare Pico SIM7080G Cat-M1 modem board](https://www.waveshare.com/pico-sim7080g-cat-m-nb-iot.htm) together.

### Requirements

* Waveshare’s Pico SIM7080G Cat-M1/NB-IoT — get it [direct from Waveshare](https://www.waveshare.com/pico-sim7080g-cat-m-nb-iot.htm).
* An HT16K33-based four-digit, seven-segment display; I used [this one](https://www.adafruit.com/product/879).
* An MCP9808 thermal sensor breakout board; here’s a [good one](https://www.adafruit.com/product/1782).
* Wires.
* One large, or two standard-size, breadboards.

### Build

This is the circuit layout:

![Cellular IoT Sample circuit layout](images/cellular.png)

### Usage

The device accepts commands in JSON form. For example:

```json
{"cmd": "num", "val": 2021}
```

The JSON string must be converted to a base64 string for transmission by SMS:

```bash
SEND_DATA=$(echo '{"cmd": "num", "val": 2021}' | base64)
```

You can then send the SMS. For example, using [Twilio Super SIM]():

```bash
curl -X POST https://supersim.twilio.com/v1/SmsCommands \
    --data-urlencode "${SIM_SID}" \
    --data-urlencode "Payload=${SEND_DATA}" \
    -u "${ACCOUNT_SID}:${ACCOUNT_AUTH_TOKEN}"
```

You will need shell variables for your Super SIM’s SID, and your Twilio account SID and authorisation token. Different brands of SIM will have different settings, of course.

Alternatively, just punch those values into the `cmd.sh` script that’s included and call it with two arguments. For example"

```bash
./cmd.sh num 9876
```

will send (base64 encoded):

```json
{"cmd": "num", "val": 9876}
```

Commands available from the device:

* `led` — flash the Pico LED. Requires a `val` object with the number of flashes as an integer.
* `num` — write a value on the display. Requires a `val` object with the number (between 0 and 9999) as an integer.
* `tmp` — Send back a Celsius temperature reading. Requires your Super SIM’s fleet to be configured with an SMS Commands webhook, or equivalent for other brands of SIM. Super SIM device-originated SMS Commands are sent to the number 000, so you will need to alter that value in the code (in `modem.cpp`). It also requires server-side code to process or display the received data.
* `flash` — flash a code on the LED. Requires a `code` object with a string pattern, eg. `LBSBL` where:
    * `L` — Long on (500ms)
    * `S` — Short on (250ms)
    * `B` - Short off (250ms)

### Credits

This project uses the following third-party code:

* [ArduinoJson](https://github.com/bblanchon/ArduinoJson) © 2014-2021, Benoit Blanchon, MIT licence.
* [Base64](https://github.com/ReneNyffenegger/cpp-base64) © 2004-2021, René Nyffenegger. ZLib licence.

---

## Phantom Slayer 1.0.2

A retro-style 3D arcade game written in C. For more details, [see this page](https://smittytone.net/pico-phantoms/).

#### Requirements

* An SSD1306-based 128x64 OLED display; I used [this display](https://www.adafruit.com/product/326).
* A piezo speaker.
* Two small buttons.
* A two-way, self-centring analog joystick; there are [lots available on eBay](https://www.ebay.co.uk/itm/New-PSP-2-Axis-Analog-Thumb-GAME-Joystick-Module-3V-5V-For-arduino-PSP/401104248437?hash=item5d63ad1e75:g:HcEAAOSwjqVZNSzw).
* Wires.
* Two standard breadboards.

### Build

![Phantom Slayer circuit layout](images/phantoms.png)

The joystick shown is not the one used, but it gives you the idea. Connect purple to the X pin, white to the Y pin.

### The Code

Build from source code, or copy `phantoms.uf2` to your Pico. Please check the SHA 256:

```
d9e4e2d28b8a5b983e1551d114cd4ae6d668858b43a9df6c92f946327a96f146
```

### The Game

See [this blog post for full details](https://blog.smittytone.net/2021/03/26/3d-arcade-action-courtesy-of-raspberry-pi-pico/).

### Credits

This games is based on a 1982 original created by Ken Kalish of Med Systems for the Tandy Color Computer. The design is Ken’s; the code is mine, and I’ve taken only a few liberties with certain details.

Phantom Slayer uses [TinyMT](https://github.com/MersenneTwister-Lab/TinyMT), copyright © 2011 Mutsuo Saito, Makoto Matsumoto, Hiroshima University and The University of Tokyo. All rights reserved. Licensed under the [three-clause BSD Licence](/phantoms/TinyMT-Licence.txt).

---

## Hunt the Wumpus 1.0.2 ##

A sample C project to build a fun game.

### Requirements

* An HT16K33-based 8x8 LED matrix; I used [this display](https://www.adafruit.com/product/1049).
* A piezo speaker.
* One small button.
* A two-way, self-centring analog joystick; there are [lots available on eBay](https://www.ebay.co.uk/itm/New-PSP-2-Axis-Analog-Thumb-GAME-Joystick-Module-3V-5V-For-arduino-PSP/401104248437?hash=item5d63ad1e75:g:HcEAAOSwjqVZNSzw).
* A red LED.
* A green LED.
* Wires.
* Two standard breadboards.

### Build

![Hunt the Wumpus circuit layout](images/wumpus.png)

* Make sure you wire the LEDs correctly: longer leg to the Pico pin, shorter leg to GND.
* The joystick shown is not the one used, but it gives you the idea. Connect white to the X pin, blue to the Y pin.

### The Code

Build from source code, or copy `wumpus.uf2` to your Pico. Please check the SHA 256:

```
65957c643df25f89ddd3f70f16131c7037e582fa04743c35b7af78b90eb0a0f5
```

### The Game

This is a fun little game to hunt the Wumpus. Move through the cave with the joystick.

A red light indicates a nearby pit — if you fall in, you’ll be killed.

A twittering sound indicates a nearby bat. If you enter its square, it will carry you to another part of the cave.

A green light indicates the Wumpus is close. Enter its square and it will eat you, but if you’re sure where it is, press the button to fire an arrow to kill it first. To fire the arrow successfully, back off and then move toward the Wumpus in the direction you want to the arrow to fly. If you miss, the beast will catch you!

### Credits

This version was based on an [Arduino version](https://github.com/coreyfaure/HuntTheWumpus-Arduino) created by Corey Faure. This is very much his game, but I have taken a few liberties with a couple of the animations and rewritten the code.

Hunt the Wumpus uses [TinyMT](https://github.com/MersenneTwister-Lab/TinyMT), copyright © 2011 Mutsuo Saito, Makoto Matsumoto, Hiroshima University and The University of Tokyo. All rights reserved. Licensed under the [three-clause BSD Licence](/wumpus/TinyMT-Licence.txt).

---

## Sensor

A sample C project. Requires an HT16K33-based 4-digit, 7-segment LED and an SGP30 TVOC sensor. I used [this sensor](https://shop.pimoroni.com/products/sgp30-air-quality-sensor-breakout) and [this display](https://www.adafruit.com/product/878):

![Sensor circuit layout](images/sensor.png)

---

## I2C

A sample C project. Requires an HT16K33-based 8x8 LED matrix. I used [this display](https://www.adafruit.com/product/1049).

![I2C sample circuit layout](images/i2c.png)

---

## Micropython

A sample MicroPython project. Requires an HT16K33-based 8x8 LED matrix.

---

## makepico 2.1.0

Pico project set up script for Z Shell. Requires Pico C SDK pre-installed.

---

## Updates

- *3 September 2021*
    - Complete *Cellular IoT Demo* — add `POST` ops and various improvements.
- *20 August 2021*
    - Add *Cellular IoT Demo*.
- *13 August 2021*
    - *makepico* now generates `main.c/cpp` and `main.h` not project-specific files.
    - *makepico*’s `CMakeLists.txt` now uses CMake 3.14.
    - *makepico*’s `main.c` now calls `stdio_init_all()`.
- *23 July 2021*
    - Spring clean *makepico* and bump to 2.0.0.
- *30 April 2021*
    - Update *makepico* to support C++ projects:
        - Add `-c` switch to create a C++ project.
        - Add a `-n` option so you can add your name for code comments.
- *6 April 2021*
    - Bump Phantom Slayer to 1.0.2
        - Move some common routines into `utils.h`/`utils.c`.
        - Update *inkey()* to return the key pressed.
        - Add another map.
- *31 March 2021*
    - Bump Phantom Slayer to 1.0.1
        - Graphics tweaks.
        - Improve Phantom movement logic.
        - Fix laser post-fire delay.
    - Bump Hunt the Wumpus to 1.0.2
        - Use TinyMT for random number generation.
    - Bump makepico to 1.2.0.
- *26 March 2021*
    - Add Phantom Slayer 1.0.0
- *25 February 2021*
    - Bump Wumpus to 1.0.1
        - Tweak sprites.
        - Improve trophy presentation.
        - Improve in-game code flow.
- *20 February 2021*
    - Add *wumpus* example.
    - Update *makepico* script:
        - Add VSCode config creation.
        - Add `-d`/`--debug` switch to add VSCode SWD debugging support.
- *5 February 2021*
    - Add *sensor* example.
- *4 February 2021*
    - Add debugger-friendly VSCode `launch.json`.
- *2 February 2021*
    - Initial release.

All source code released under the MIT Licence. Copyright © 2021, Tony Smith (@smittytone).