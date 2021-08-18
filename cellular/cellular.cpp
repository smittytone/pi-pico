/*
 * cellular::main for Raspberry Pi Pico
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
using std::stringstream;


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
    I2C::setup();

    // Initialize the display
    display.init();
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

                    // ...and process it for commands but getting the message body...
                    string data = Utils::split_msg(msg, 2);

                    // ...decoding the base64 to a JSON string...
                    string json = base64_decode(data);

                    // ...and parsing the JSON
                    DynamicJsonDocument doc(128);
                    DeserializationError err = deserializeJson(doc, json.c_str());
                    if (err == DeserializationError::Ok) {
                        string cmd = doc["cmd"];
                        uint32_t value = doc["val"];

                        // Check for commands
                        if (cmd == "LED" || cmd == "led") process_command_led(value);
                        if (cmd == "NUM" || cmd == "num") process_command_num(value);
                        if (cmd == "TMP" || cmd == "tmp") process_command_tmp();
                    }

                    // Delete all SMSs now we're done with them
                    modem.send_at("AT+CMGD=" + num + ",4");
                }
            }
        }
    }
}

void process_command_led(uint32_t blinks) {
    #ifdef DEBUG
    printf("Received LED command: %i blinks\n", blinks);
    #endif

    blink_led(blinks);
}

void process_command_num(uint32_t number) {
    #ifdef DEBUG
    printf("Received NUM command: %i\n", number);
    #endif

    // Get the BCD data and use it to populate
    // the display's four digits
    uint32_t bcd_val = Utils::bcd(number);
    display.clear();
    display.set_number((bcd_val >> 12) & 0x0F, 0, false);
    display.set_number((bcd_val >> 8) & 0x0F, 1, false);
    display.set_number((bcd_val >> 4) & 0x0F, 2, false);
    display.set_number(bcd_val & 0x0F, 3, false);
    display.draw();
}

void process_command_tmp() {
    #ifdef DEBUG
    printf("Received TMP command\n");
    #endif

    stringstream stream;
    stream << std::fixed << std::setprecision(2) << sensor.read_temp();
    string temp = stream.str();

    if (modem.send_at("AT+CMGS=\"000\"", ">")) {
        // '>' is the prompt sent by the modem to signal that
        // it's waiting to receive the message text.
        // 'chr(26)' is the code for ctrl-z, which the modem
        // uses as an end-of-message marker
        string r = modem.send_at_response(temp + "\x1A");
    }
}