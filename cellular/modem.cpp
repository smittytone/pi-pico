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


/**
    Construct a SIM7080G modem instance.
 */
Sim7080G::Sim7080G(string network_apn) {
    // Clear the input bufffer
    clear_buffer();

    // Save the APN for later
    apn = network_apn;
    if (apn == "") apn = "super";
}


/**
    Start up the modem.

    - Returns: `true` if the modem is ready, otherwise `false`.
 */
bool Sim7080G::start_modem() {
    if (init_modem()) {
        init_network();
        return true;
    } else {
        // Signal with two flashes we couldn't
        // start the modem
        blink_err_code(ERR_CODE_MODEM_START);
        sleep_ms(1000);
    }

    return false;
}


/**
    Check the modem is ready by periodically sending an AT command
    until we receive a valid response. Power on the modem on the
    first failure.

    - Returns: `true` if the modem is ready, otherwise `false`.
 */
bool Sim7080G::init_modem() {
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
            sleep_ms(2500);
        }
    }

    return false;
}


/**
    Initialise the modem: set up Cat-M1 usage and write the
    APN for Super SIM usage.
 */
void Sim7080G::init_network() {
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


/**
    Send an AT command to the modem and check the response.

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
    // Write out the AT command, converting to
    // a C string for the Pico SDK
    #ifdef DEBUG
    printf("SEND_AT  CMD: %s\n", cmd.c_str());
    #endif

    string data_out = cmd + "\r\n";
    //char c_data_out[data_out.length() + 1];
    //strcpy(c_data_out, data_out.c_str());
    uart_puts(MODEM_UART, data_out.c_str());

    // Read the buffer
    read_buffer(timeout);

    // Return response as string
    if (rx_ptr > &uart_buffer[0]) {
        string r = buffer_to_string();
        #ifdef DEBUG
        printf("SEND_AT RESP:\n%s", r.c_str());
        #endif
        return r;
        }
    return "zzz";
}


/**
    Read the UART RX for a period of time.

    - Parameters:
        - timeout: milliseconds to wait for response data.
 */
void Sim7080G::read_buffer(uint32_t timeout) {
    // Reset the read pointer
    clear_buffer();
    uint8_t* buffer_start = &uart_buffer[0];
    rx_ptr = buffer_start;

    uint32_t now = time_us_32();

    while ((time_us_32() - now < timeout * 1000) && (rx_ptr - buffer_start < UART_BUFFER_SIZE)) {
        if (uart_is_readable(MODEM_UART) > 0) {
            uart_read_blocking(MODEM_UART, rx_ptr, 1);
            rx_ptr++;
        }
    }
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
    string new_string(uart_buffer, rx_ptr);
    return new_string;
}


string Sim7080G::listen(uint32_t timeout) {
    // Listen for an incomimg message
    read_buffer(timeout);

    // Return response as string
    return buffer_to_string();
}
