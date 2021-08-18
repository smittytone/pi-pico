/*
 * cellular::modem for Raspberry Pi Pico
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


/**
    Construct a SIM7080G modem instance.
 */
Sim7080G::Sim7080G(string network_apn) {
    // Clear the input bufffer
    clear_buffer();

    // Save the APN for later
    apn = network_apn;
    if (apn.length() == 0) apn = "super";
}

/**
    Start up the modem.

    - Returns: `true` if the modem is ready, otherwise `false`.
 */
bool Sim7080G::start_modem() {
    if (boot_modem()) {
        config_modem();
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
bool Sim7080G::boot_modem() {
    bool state = false;
    uint32_t count = 0;
    uint32_t start_time = time_us_32();

    do {
        if (send_at("ATE1", "OK", 5000)) {
            #ifdef DEBUG
            printf("Modem ready after %ims\n", (time_us_32() - start_time) / 1000);
            #endif

            return true;
        }

        // Toggle the PWR_EN pin once
        if (!state) {
            toggle_module_power();
            state = true;
        }

        // Wait a bit
        sleep_ms(5000);
        count++;
    } while (count < 20);

    return false;
}

/**
    Initialise the modem: set up Cat-M1 usage and write the
    APN for Super SIM usage.
 */
void Sim7080G::config_modem() {
    // Set error reporting to 2
    send_at("AT+CMEE=2");

    // Set modem to text mode
    send_at("AT+CMGF=1");

    // Select LTE-only mode
    send_at("AT+CNMP=38");

    // Select Cat-M only mode
    send_at("AT+CMNB=1");

    // Set the APN
    send_at("AT+CGDCONT=1,\"IP\",\"" + apn + "\"");
}


/**
    Check network connection.
 */
bool Sim7080G::check_network() {

    bool is_connected = false;
    string response = send_at_response("AT+COPS?");
    string line = Utils::split_msg(response, 1);
    if (line.find("+COPS:") != string::npos) {
        uint32_t pos = line.find(",");
        // ',' will be missing if the modem is not connected,
        // ie. there is no operator value in the AT+COPS? response
        is_connected = (pos != string::npos);
    }

    return is_connected;
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

    - Returns: The bytes received as a string, or `ERROR`.
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
        string response = buffer_to_string();
        #ifdef DEBUG
        printf("SEND_AT RESP:\n%s", response.c_str());
        #endif
        return response;
    }

    return "ERROR";
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

/**
    Listen for period for an incomimg message

    - Parameters:
        - timeout: The waiting period.

    - Returns: The recieved bytes as a string
 */
string Sim7080G::listen(uint32_t timeout) {
    //
    read_buffer(timeout);

    // Return response as string
    return buffer_to_string();
}
