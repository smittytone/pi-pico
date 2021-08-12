/*
 * cellular for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "cellular.h"

using std::string;
using std::vector;


/*
 * GLOBALS
 */
Sim7080G modem = Sim7080G("super");
MCP9808 sensor = MCP9808(0x18);

int main() {

    // DEBUG
    stdio_init_all();

    // Set up the hardware
    setup();

    // Fire up the modem
    if (modem.init_modem()) {
        // Light the LED
        led_on();

        // Start to listen for commands
        listen();
    } else {
        // Error! Flash the LED five times, turn it off and exit
        blink_led(5);
        gpio_put(PIN_LED, false);
    }

    return 0;
}


/**
    Umbrella setup routine.
 */
void setup() {
    setup_led();
    setup_i2c();
    setup_uart();
    setup_modem_power_pin();
}


/**
    Set up the UART link to the modem.
 */
void setup_uart() {
    // Initialise UART 0
    uart_init(MODEM_UART, 115200);

    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(PIN_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_UART_RX, GPIO_FUNC_UART);
}


/*
 * LED Functions
 */
void setup_led() {
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    led_off();
}

void led_on() {
    gpio_put(PIN_LED, true);
}

void led_off() {
    gpio_put(PIN_LED, false);
}

/**
    Blink the Pico LED a specified number of times, leaving it
    on at the end.

    - Parameters:
        - blinks: The number of flashes.
 */
void blink_led(uint32_t blinks) {
    for (uint32_t i = 0 ; i < blinks ; ++i) {
        gpio_put(PIN_LED, false);
        sleep_ms(250);
        gpio_put(PIN_LED, true);
        sleep_ms(250);
    }
}


/*
 * Modem PWR_EN Functions
 */
void setup_modem_power_pin() {
    gpio_init(PIN_MODEM_PWR);
    gpio_set_dir(PIN_MODEM_PWR, GPIO_OUT);
    gpio_put(PIN_MODEM_PWR, false);
}


/*
 * I2C Functions
 */
void setup_i2c() {
    // Initialize the I2C bus for the display and sensor
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_GPIO);
    gpio_pull_up(SCL_GPIO);

    // TODO
    // Initialize the display
    // Initialize the sensor
}

void i2c_write_byte(uint8_t address, uint8_t byte) {
    // Convenience function to write a single byte to the bus
    i2c_write_blocking(I2C_PORT, address, &byte, 1, false);
}

void i2c_write_block(uint8_t address, uint8_t *data, uint8_t count) {
    // Convenience function to write a 'count' bytes to the bus
    i2c_write_blocking(I2C_PORT, address, data, count, false);
}

void i2c_read_block(uint8_t address, uint8_t *data, uint8_t count) {
    // Convenience function to read 'count' bytes from the bus
    i2c_read_blocking(I2C_PORT, address, data, count, false);
}


/*
 * MAIN FUNCTIONS
 */
void listen() {
    while (true) {
        // Check for a response from the modem
        string response = modem.listen(5000);

        if (response.length() > 0) {
            vector<string> lines = split_to_lines(response);
            for (uint32_t i = 0 ; i < lines.size() ; ++i) {
                string line = lines[i];
                if (line.find("CMTI") != string::npos) {
                    // We received an SMS, so get it...
                    string num = modem.get_sms_number(line);
                    string msg = modem.send_at_response("AT+CMGR=" + num, 5000);

                    // ...and process it for commands
                    string cmd = modem.split_msg(msg, 2);
                    modem.send_at_response("AT+CMGD=" + num + ",4", 5000);
                    if (cmd.find("LED=") == 0) process_command_led(cmd);
                    if (cmd.find("TMP") == 0) process_command_tmp();

                    // Delete all SMSs now we're done with them
                    modem.send_at("AT+CMGD=" + num + ",4", "OK", 2000);
                }
            }
        }
    }
}


void process_command_led(string msg) {
    string s_blinks = msg.substr(4);
    uint32_t i_blinks = std::stoi(s_blinks);
    blink_led(i_blinks);
}


void process_command_tmp() {
    string s_temp(std::to_string(sensor.read_temp()));
    if (modem.send_at("AT+CMGS=\"000\"", ">", 2000)) {
        // '>' is the prompt sent by the modem to signal that
        // it's waiting to receive the message text.
        // 'chr(26)' is the code for ctrl-z, which the modem
        // uses as an end-of-message marker
        string r = modem.send_at_response(s_temp + "\x1A", 2000);
    }
}


vector<string> split_to_lines(string str) {
    vector<string> result;
    while (str.size()) {
        int index = str.find("\r");
        if (index != string::npos){
            result.push_back(str.substr(0, index));
            str = str.substr(index + 2);
            if (str.size() == 0) result.push_back(str);
        } else {
            result.push_back(str);
            break;
        }
    }

    return result;
}