/*
 * cellular::main for Raspberry Pi Pico
 *
 * @version     1.0.2
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "cellular.h"

using std::string;
using std::vector;
using std::stringstream;


/*
 * GLOBALS
 */
Sim7080G modem = Sim7080G();
MCP9808 sensor = MCP9808();
HT16K33_Segment display = HT16K33_Segment();


/*
 * UART FUNCTIONS
 */

/**
 * @brief Initialise a UART bus.
 */
void setup_uart() {
    // Initialise UART 0
    uart_init(MODEM_UART, 115200);

    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(PIN_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_UART_RX, GPIO_FUNC_UART);

    // Switch off flow control -- not used
    uart_set_hw_flow(MODEM_UART, false, false);
}


/*
 * LED FUNCTIONS
 */

/**
 * @brief Configure the on-board LED.
 */
void setup_led() {
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    led_off();
}


/**
 * @brief Turn the on-board LED on.
 */
void led_on() {
    led_set();
}


/**
 * @brief Turn the on-board LED off.
 */
void led_off() {
    led_set(false);
}


/**
 * @brief Set the on-board LED's state.
 */
void led_set(bool state) {
    gpio_put(PICO_DEFAULT_LED_PIN, state);
}


/**
 * @brief Blink the Pico LED a specified number of times, leaving it
 *        on at the end.
 *
 * @param blinks: The number of flashes.
 */
void blink_led(uint32_t blinks) {
    for (uint32_t i = 0 ; i < blinks ; ++i) {
        led_off();
        sleep_ms(250);
        led_on();
        sleep_ms(250);
    }
}


/**
 * @brief Flash a error code sequence on the LED.
 *        eg. "LBSBS" - Long, Blank, Short, Blank, Short.
 *
 * @param code: Sequence of L, S or B.
 */
void blink_err_code(string code) {
    for (uint32_t i = 0 ; i < code.length() ; ++i) {
        switch (code[i]) {
            case 'L':
                // On for 500ms
                led_on();
                sleep_ms(250);
                break;
            case 'S':
                // On for 250ms
                led_on();
                break;
            case 'B':
                // Off for 250ms
                led_off();
        }

        sleep_ms(250);
    }

    led_off();
}


/*
 * GPIO FUNCTIONS
 */

/**
 * @brief Initialise the modem power pin.
 */
void setup_modem_power_pin() {
    gpio_init(PIN_MODEM_PWR);
    gpio_set_dir(PIN_MODEM_PWR, GPIO_OUT);
    gpio_put(PIN_MODEM_PWR, false);
}


/*
 * I2C FUNCTIONS
 */

/**
 * @brief Initialise the modem power pin.
 */
void setup_i2c() {
    // Initialize the I2C bus for the display and sensor
    I2C::setup();

    // Initialize the HT16K33 display
    display.init();
}


/*
 * MAIN FUNCTIONS
 */

/**
 * @brief Umbrella setup routine.
 */
void setup() {
    setup_led();
    setup_i2c();
    setup_uart();
    setup_modem_power_pin();
}


/**
 * @brief Loop and wait for incoming SMS messages, which are parsed and
 *        and commands they contain are processed.
 *
 *        Could be more sophisticated, but it works!
 */
void listen() {
    bool do_monitor_tmp = false;
    bool do_monitor_rssi = false;
    while (true) {
        // Check for a response from the modem
        string response = modem.listen(5000);
        if (response != "ERROR") {
            const vector<string> lines = Utils::split_to_lines(response);
            for (uint32_t i = 0 ; i < lines.size() ; ++i) {
                const string line = lines[i];
                if (line.length() == 0) continue;
                if (line.find("+CMTI") != string::npos) {
                    // We received an SMS, so get it...
                    const string num = Utils::get_sms_number(line);
                    const string msg = modem.send_at_response("AT+CMGR=" + num);

                    // ...and process it for commands but getting the message body...
                    const string data = Utils::split_msg(msg, 2);

                    // ...decoding the base64 to a JSON string...
                    const string json = base64_decode(data);

                    // ...and parsing the JSON
                    DynamicJsonDocument doc(128);
                    DeserializationError err = deserializeJson(doc, json.c_str());
                    if (err == DeserializationError::Ok) {
                        const string cmd = Utils::uppercase(doc["cmd"]);

                        // Check for commands
                        if (cmd == "LED") {
                            process_command_led(doc["val"]);
                        } else if (cmd == "NUM") {
                            process_command_num(doc["val"]);
                        } else if (cmd == "TMP") {
                            process_command_tmp(true);
                        } else if (cmd == "GET") {
                            process_command_get();
                        } else if (cmd == "POST") {
                            process_command_post(doc["data"]);
                        } else if (cmd == "FLASH") {
                            process_command_flash(doc["code"]);
                        } else if (cmd == "AT") {
                            process_command_at(doc["code"]);
                        } else if (cmd == "TMPCON") {
                            do_monitor_tmp = !do_monitor_tmp;
                            if (do_monitor_tmp && do_monitor_rssi) do_monitor_rssi = false;
                        } else if (cmd == "RSSI") {
                            do_monitor_rssi = !do_monitor_rssi;
                            if (do_monitor_rssi && do_monitor_tmp) do_monitor_tmp = false;
                        } else {
                            #ifdef DEBUG
                            printf("ERROR -- Unknown command: %s\n", cmd.c_str());
                            #endif
                        }
                    }

                    // Delete all SMS now we're done with them
                    modem.send_at("AT+CMGD=,4");
                }
            }
        }

        if (do_monitor_tmp) process_command_tmp(false);
        if (do_monitor_rssi) process_command_rssi();
    }
}


/**
 * @brief Blink the LED a number of times.
 *
 * @param blinks: The number of blinks.
 */
void process_command_led(uint32_t blinks) {
    #ifdef DEBUG
    printf("Received LED command: %i blink(s)\n", blinks);
    #endif

    if (blinks < 1 || blinks > 100) blinks = 1;
    blink_led(blinks);
}


/**
 * @brief Display a number on the LED.
 *
 * @param blinks: The number to show.
 */
void process_command_num(uint32_t number) {
    #ifdef DEBUG
    printf("Received NUM command: %i\n", number);
    #endif

    // Get the BCD data and use it to populate
    // the display's four digits
    if (number < 0 || number > 9999) number = 9999;
    const uint32_t bcd_val = Utils::bcd(number);
    display.clear();
    display.set_number((bcd_val >> 12) & 0x0F, 0, false);
    display.set_number((bcd_val >> 8) & 0x0F, 1, false);
    display.set_number((bcd_val >> 4) & 0x0F, 2, false);
    display.set_number(bcd_val & 0x0F, 3, false);
    display.draw();
}


/**
 * @brief Display a temperature on the LED.
 *
 * @param do_send: Should an HTTP request be issued too.
 */
void process_command_tmp(bool do_send) {
    #ifdef DEBUG
    printf("Received TMP command\n");
    #endif

    // Convert the temperature value (a float) to a string value
    // fixed to two decimal places
    stringstream stream;
    stream << std::fixed << std::setprecision(2) << sensor.read_temp();
    const string temp = stream.str();

    if (do_send) {
        #ifdef DEBUG
        printf("Received TMP command\n");
        #endif

        if (modem.send_at("AT+CMGS=\"000\"", ">")) {
            // '>' is the prompt sent by the modem to signal that
            // it's waiting to receive the message text.
            // 'chr(26)' is the code for ctrl-z, which the modem
            // uses as an end-of-message marker
            string r = modem.send_at_response(temp + "\x1A");

            // NOTE For some reason TBD, this triggers a +CMS ERROR: 500,
            //      but the text message gets through
        }
    }

    // Display the temperature on the LED
    uint32_t digit = 0;
    char previous_char = 0;
    char current_char = 0;
    for (uint32_t i = 0 ; (i < temp.length() || digit == 3) ; ++i) {
        current_char = temp[i];
        if (current_char == '.' && digit > 0) {
            display.set_alpha(previous_char, digit - 1, true);
        } else {
            display.set_alpha(current_char, digit);
            previous_char = current_char;
            digit++;
        }
    }

    // Add a final 'c' and update the display
    display.set_alpha('c', 3).draw();
}


/**
 * @brief Display the modem's RSSI on the LED.
 */
void process_command_rssi() {
    // Get the current RSSI and separate out the RSSI value
    string response = modem.send_at_response("AT+CSQ");
    vector<string> parts = Utils::split_to_lines(response, ": ");
    if (parts.size() > 1) {
        vector<string> values = Utils::split_to_lines(parts[1], ",");
        if (values.size() > 1) {
            // Convert to integer
            uint32_t rssi = std::stoi(values[0]);

            // Check for the 'unknown RSSI' value -- just show a ?
            if (rssi == 99) {
                display.clear().set_alpha('?', 0).set_alpha('?', 1).set_alpha('?', 2).set_alpha('?', 3).draw();
                return;
            }

            // Convert the RSSI from an internal value to dBm
            // NOTE Internal values 0, 1 and 31 are fixed dbm values,
            //      but 2-29 are scale values
            switch(rssi) {
                case 0:
                    process_command_num(115);
                    break;
                case 1:
                    process_command_num(111);
                    break;
                case 31:
                    process_command_num(56);
                    break;
                default:
                    process_command_num(((28 - rssi) * 2) + 54);
            }

            // Add a minus sign
            display.set_glyph(0x40, 0).draw();
        }
    }
}


/**
 * @brief Pass a supplied AT command string to the modem.
 *
 * @param cmd: The command string.
 */
void process_command_at(string cmd) {
    const string response = modem.send_at_response(cmd);
    #ifdef DEBUG
    printf("Response:\n");
    printf(response.c_str());
    printf("\n");
    #endif
}


/**
 * @brief Issue a sample GET request.
 */
void process_command_get() {
    #ifdef DEBUG
    printf("Requesting data...\n");
    #endif

    const string server = "http://jsonplaceholder.typicode.com";
    const string endpoint_path = "/todos/1";
    process_request(server, endpoint_path);
}


/**
 * @brief Issue a sample POST request.
 *
 * @param data: The POST request's body string.
 */
void process_command_post(string data) {
    #ifdef DEBUG
    printf("Sending data...\n");
    #endif

    const string server = "<YOUR_SERVER>";
    const string endpoint_path = "<YOUR_ENDPOINT>";
    process_request(server, endpoint_path, data);
}

/**
 * @brief Generic HTTP request handler.
 *
 * @param server: The target server domain prefixed with the protool, eg.
 *                `https://example.com`.
 * @param path:   The target endpoint path.
 * @param data:   The data to send.
 */
void process_request(string server, string path, string data) {
    // Attempt to open a data connection
    bool send_success = false;
    if (modem.open_data_conn()) {
        if (data.length() > 0) {
            send_success = modem.send_data(server, path, data);
        } else {
            send_success = modem.get_data(server, path);
        }

        if (send_success) {
            // Attempt to decode the received JSON. You may need to adjust
            // the memory allocation (default: 1024) for large JSON responses
            DynamicJsonDocument doc(1024);
            DeserializationError err = deserializeJson(doc, modem.data.c_str());

            if (err == DeserializationError::Ok) {
                // Make use of the data: extract a value and display it
                #ifdef DEBUG
                const string title = doc["title"];
                printf("DATA RETURNED: %s\n", title.c_str());
                #endif

                process_command_num(doc["userId"]);
            } else {
                #ifdef DEBUG
                printf("Malformed JSON received: error %s\n%s\n", err.c_str(), modem.data.c_str());
                #endif
            }
        }

        // Close the open connection
        modem.close_data_conn();
    }
}


/**
 * @brief Blink the LED to a specified pattern.
 *
 * @param code: The flash pattern.
 */
void process_command_flash(string code) {
    #ifdef DEBUG
    printf("Received FLASH command -- sequence: %s\n", code.c_str());
    #endif

    blink_err_code(code);
    sleep_ms(1000);
    led_on();
}

/*
 * RUNTIME START
 */
int main() {
    // DEBUG
    #ifdef DEBUG
    stdio_init_all();
    #endif

    // Set up the hardware
    setup();

    // Fire up the modem
    #ifdef DEBUG
    printf("Starting modem...\n");
    #endif

    if (modem.start_modem()) {
        // Check the network
        bool state = true;
        while (!modem.check_network()) {
            if (state) {
                led_on();
            } else {
                led_off();
            }

            state = !state;
            sleep_ms(250);
        }

        // Light the LED
        led_on();

        // Start to listen for commands
        #ifdef DEBUG
        printf("Listening for commands...\n");
        #endif

        listen();
    } else {
        // Error! Flash the LED five times, turn it off and exit
        blink_err_code(ERR_CODE_NO_MODEM);
        gpio_put(PIN_LED, false);
    }

    return 0;
}