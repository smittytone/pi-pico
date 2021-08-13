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
Sim7080G modem = Sim7080G();
MCP9808 sensor = MCP9808();
HT16K33_Segment display = HT16K33_Segment();


/**
    UART FUNCTIONS
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

/**
    Flash a error code sequence on the LED.

    eg. "LBSBS" - Long, Blank, Short, Blank, Short.

    - Parameters:
        - code: Sequence of L, S or B.
 */
void blink_err_code(string code) {
    for (uint32_t i = 0 ; i < code.length() ; ++i) {
        switch (code[i]) {
            case 'L':
                gpio_put(PIN_LED, true);
                sleep_ms(250);
            case 'S':
                gpio_put(PIN_LED, true);
                sleep_ms(250);
                break;
            default:
                sleep_ms(250);
                break;
        }
        gpio_put(PIN_LED, false);
    }
}


/*
 * MODEM PWR_EN FUNCTIONS
 */
void setup_modem_power_pin() {
    gpio_init(PIN_MODEM_PWR);
    gpio_set_dir(PIN_MODEM_PWR, GPIO_OUT);
    gpio_put(PIN_MODEM_PWR, false);
}


/*
 * I2C FUNCTIONS
 */
void setup_i2c() {
    // Initialize the I2C bus for the display and sensor
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_GPIO);
    gpio_pull_up(SCL_GPIO);

    // Initialize the display
    display.init();
}

/**
    Convenience function to write a single byte to the bus
 */
void i2c_write_byte(uint8_t address, uint8_t byte) {
    i2c_write_blocking(I2C_PORT, address, &byte, 1, false);
}

/**
    Convenience function to write a 'count' bytes to the bus
 */
void i2c_write_block(uint8_t address, uint8_t *data, uint8_t count) {
    i2c_write_blocking(I2C_PORT, address, data, count, false);
}

/**
    Convenience function to read 'count' bytes from the bus
 */
void i2c_read_block(uint8_t address, uint8_t *data, uint8_t count) {
    i2c_read_blocking(I2C_PORT, address, data, count, false);
}


/*
 * MAIN FUNCTIONS
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
        // Light the LED
        led_on();

        // Start to listen for commands
        #ifdef DEBUG
        printf("Listening...\n");
        #endif
        listen();
    } else {
        // Error! Flash the LED five times, turn it off and exit
        blink_err_code(ERR_CODE_GEN_FAIL);
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
    Loop and wait for incoming SMS messages, which are parsed and
    and commands they contain are processed.

    Could be more sophisticated, but it works!
 */
void listen() {
    while (true) {
        // Check for a response from the modem
        string response = modem.listen(5000);
        if (response != "ERROR") {
            vector<string> lines = Utils::split_to_lines(response);
            for (uint32_t i = 0 ; i < lines.size() ; ++i) {
                string line = lines[i];
                if (line.length() == 0) continue;

                #ifdef DEBUG
                printf("LINE %i: %s\n", i, line.c_str());
                #endif

                if (line.find("+CMTI") != string::npos) {
                    // We received an SMS, so get it...
                    string num = Utils::get_sms_number(line);
                    string msg = modem.send_at_response("AT+CMGR=" + num);

                    // ...and process it for commands
                    string cmd = Utils::split_msg(msg, 2);
                    if (cmd.find("LED=") == 0) process_command_led(cmd);
                    if (cmd.find("NUM=") == 0) process_command_num(cmd);
                    if (cmd.find("TMP") == 0) process_command_tmp();

                    // Delete all SMSs now we're done with them
                    modem.send_at("AT+CMGD=" + num + ",4");
                }
            }
        }
    }
}


void process_command_led(string msg) {
    string s_blinks = msg.substr(4);

    #ifdef DEBUG
    printf("Recieved LED command: %s blinks\n", s_blinks.c_str());
    #endif

    uint32_t i_blinks = std::stoi(s_blinks);
    blink_led(i_blinks);
}


void process_command_num(string msg) {
    string number = msg.substr(4);

    #ifdef DEBUG
    printf("Recieved NUM command: %s\n", number.c_str());
    #endif

    display.clear();
    uint32_t digit = 3;
    for (uint32_t i = number.length() - 1; i >= 0 ; --i) {
        char n = number[i];
        printf("%i. %i, %c\n", digit, number[i], n);
        display.set_alpha(n, digit, false);
        digit--;
        if (digit > 4) break;
    }

    if (digit >= 0 && digit < 4) {
        for (uint32_t i = 0; i <= digit ; ++i) {
            display.set_number('0', i, false);
        }
    }

    display.draw();
}


void process_command_tmp() {
    #ifdef DEBUG
    printf("Recieved TMP command\n");
    #endif

    // string s_temp(std::to_string(sensor.read_temp()));

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << sensor.read_temp();
    string s_temp = stream.str();

    if (modem.send_at("AT+CMGS=\"000\"", ">")) {
        // '>' is the prompt sent by the modem to signal that
        // it's waiting to receive the message text.
        // 'chr(26)' is the code for ctrl-z, which the modem
        // uses as an end-of-message marker
        string r = modem.send_at_response(s_temp + "\x1A");
    }
}