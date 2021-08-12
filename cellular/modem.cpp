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


Sim7080G::Sim7080G() {
    clear_buffer();
}


/**
    Send an AT command to the modem.

    - Parameters:
        - cmd:     pointer to the command string.
        - back:    pointer to a substring expected in the
                   response.
        - timeout: milliseconds to wait for response data.

    - Returns: `true` if the expected substring was in the response,
               otherwise `false`.
 */
bool Sim7080G::send_at(string cmd, string back, uint32_t timeout) {
    string response = send_at_response(cmd, timeout);
    return (response.length() > 0 && response.find(back) != string::npos);
}

/**
    Send an AT command to the modem.

    - Parameters:
        - cmd:     pointer to the command string.
        - timeout: milliseconds to wait for response data.

    - Returns: The number of bytes received.
 */
string Sim7080G::send_at_response(string cmd, uint32_t timeout) {
    // Write out the AT command
    string data_out = cmd + "\r\n";
    char c_data_out[data_out.length()];
    strcpy(c_data_out, data_out.c_str());
    uart_write_blocking(MODEM_UART, (uint8_t *)c_data_out, data_out.length());

    // Read the response
    uint8_t* buffer_start = &uart_buffer[0];
    rx_ptr = buffer_start;
    uint32_t now = time_us_32();
    while (time_us_32() - now < timeout || rx_ptr - buffer_start >= UART_BUFFER_SIZE) {
        if (uart_is_readable(MODEM_UART) > 0) {
            uart_read_blocking(MODEM_UART, rx_ptr, 1);
        }
    }

    // Return response as string
    return buffer_to_string();
}


/**
    Clear the RX buffer with zeroes.
 */
void Sim7080G::clear_buffer() {
    for (uint32_t i = 0 ; i < UART_BUFFER_SIZE ; ++i) {
        uart_buffer[i] = 0;
    }
}

/**
    Convert the buffer to a string.
 */
string Sim7080G::buffer_to_string() {
    string new_string = "";

    for (uint32_t i = 0 ; i < (rx_ptr - &uart_buffer[0]) ; ++i) {
        new_string += uart_buffer[i];
    }

    return new_string;
}

/**
    Initialise the modem.

    - Returns: `true` if the modem is ready, otherwise `false`.
 */
bool Sim7080G::init_modem() {
    if (start_modem()) {
        init_network("super");
        return true;
    }

    return false;
}


/**
    Check the modem is ready by periodically sending an AT command
    until we receive a valid response. Power on the modem on the
    first failure.

    - Returns: `true` if the modem is ready, otherwise `false`.
 */
bool Sim7080G::start_modem() {
    bool state = false;

    for (uint32_t i = 0 ; i < 15 ; ++i) {
        if (send_at("ATE1", "OK", 5000)) {
            return true;
        } else {
            // Toggle the PWR_EN pin once
            if (!state) {
                toggle_module_power();
                state = true;
            }

            // Wait at least 2.5s
            sleep_ms(500);
        }
    }

    return false;
}


/**
    Initialise the modem: set up Cat-M1 usage and write the
    APN for Super SIM usage.
 */
void Sim7080G::init_network(string apn) {
    // Set error reporting to 2
    send_at("AT+CMEE=2", "OK", 2000);

    // Set modem to text mode
    send_at("AT+CMGF=1", "OK", 2000);

    // Select LTE-only mode
    send_at("AT+CNMP=38", "OK", 2000);

    // Select Cat-M only mode
    send_at("AT+CMNB=1", "OK", 2000);

    // Set the APN
    send_at("AT+CGDCONT=1,\"IP\",\"" + apn + "\"", "OK", 2000);
}


/**
    Toggle the modem power line.
 */
void Sim7080G::toggle_module_power() {
    // Power the pin
    gpio_put(PIN_MODEM_PWR, true);

    // Wait at least 1.5 seconds
    sleep_ms(1500);

    // Ground the pin
    gpio_put(PIN_MODEM_PWR, false);
}


void Sim7080G::listen() {
    uint8_t* buffer_start = &uart_buffer[0];
    rx_ptr = buffer_start;

    while (true) {
        uint32_t now = time_us_32();
        while (time_us_32() - now < 5000 || rx_ptr - buffer_start >= UART_BUFFER_SIZE) {
            if (uart_is_readable(MODEM_UART) > 0) {
                uart_read_blocking(MODEM_UART, rx_ptr, 1);
            }
        }

        if (rx_ptr - buffer_start > 0) {
            string decoded = buffer_to_string();
            Vec<string> lines = string_to_lines(decoded);

            for (uint32_t i = 0 ; i < lines.length() ; ++i) {
                string line = lines[i];
                if (line.find("CMTI") != string::npos) {
                    // We received an SMS, so get it...
                    string num = get_sms_number(line);
                    string msg = send_at_response("AT+CMGR=" + num, "OK", 5000);

                    // ...and process it for commands
                    string cmd = split_msg(msg, 2);
                    send_at_response("AT+CMGD=" + num + ",4", 5000);
                    if (cmd.find("LED=") == 0) process_command_led(cmd);
                    if (cmd.find("NUM=") == 0) process_command_num(cmd);

                    // Delete all SMSs now we're done with them
                    send_at("AT+CMGD=" + num + ",4", "OK");
                }
            }

            // Reset the input buffer
            rx_ptr = buffer_start;
        }
    }
}


string Sim7080G::split_msg(string msg, uint32_t want_line) {
    uint32_t count = 0;
    uint32_t pos = msg.find("\r\n");
    string line = msg.substr(0, pos);
    do {
        if (count == want_line) return line;
    } while ((pos + 2 < msg.length()) && (line == msg.substr(pos + 2, msg.find("\r\n"))));

    return "";
}


/**
    Get a number from the end of a CMTI line.

    - Parameters:
        - line: The target line.

    - Returns: A pointer to the start of the number, or null
 */
char* get_sms_number(char* line) {
    char* ptr = strstr(line, ",");
    if (ptr) return ptr + 1;
    return NULL;
}