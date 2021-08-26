/*
 * cellular::modem for Raspberry Pi Pico
 *
 * @version     1.0.1
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

    // Initialise properties
    is_header_set = false;
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
        if (send_at("ATE1")) {
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
        sleep_ms(4000);
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

    // Set the SSL version
    send_at("AT+CSSLCFG=\"sslversion\",1,3");

    // Set SSL no verify
    send_at("AT+SHSSL=1,\"\"");

    // Set HTTPS request parameters
    send_at("AT+SHCONF=\"BODYLEN\",1024");
    send_at("AT+SHCONF=\"HEADERLEN\",350");

    // Delete left-over SMS
    send_at("AT+CMGD=,4");

    #ifdef DEBUG
    printf("Modem configured for Cat-M and Super SIM");
    #endif
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

        #ifdef DEBUG
        if (is_connected) printf("Network information: %s", line.c_str());
        #endif
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
    string data_out = cmd + "\r\n";
    uart_puts(MODEM_UART, data_out.c_str());

    // Read the buffer
    read_buffer(timeout);

    // Return response as string
    if (rx_ptr > &uart_buffer[0]) {
        return buffer_to_string();
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

    #ifdef DEBUG
    debug_output(buffer_to_string());
    #endif
}

/**
    Output IO for debugging
 */
void Sim7080G::debug_output(string msg) {
    vector<string> lines = Utils::split_to_lines(msg);
    for (uint32_t i = 0 ; i < lines.size() ; ++i) {
        printf(">>> %s", lines[i].c_str());
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
    Listen for period for an incomimg message.

    - Parameters:
        - timeout: The waiting period.

    - Returns: The recieved bytes as a string
 */
string Sim7080G::listen(uint32_t timeout) {
    // Read the buffer
    read_buffer(timeout);

    // Return response as string
    return buffer_to_string();
}

/**
    Open a data connection.
 */
bool Sim7080G::open_data_conn() {
    // Activate a data connection using PDP 0,
    // but first check it's not already open
    string response = send_at_response("AT+CNACT?");
    string line = Utils::split_msg(response, 1);
    string status = Utils::get_field_value(line, 1);
    bool success = false;

    if (status == "0") {
        // Inactive data connection so start one up
        success = send_at("AT+CNACT=0,1", "ACTIVE", 2000);
    } else if (status == "1" || status == "2") {
        success = true;
    }

    #ifdef DEBUG
    string base = "Data connection ";
    base += (success ? "active" : "inactive");
    printf("%s", base.c_str());
    #endif

    return success;
}

/**
    Close an open data connection.
 */
void Sim7080G::close_data_conn() {
    // Deactivate the connection
    send_at("AT+CNACT=0,0");

    #ifdef DEBUG
    printf("Data connection inactive");
    #endif
}

bool Sim7080G::start_session(string server) {
    // Deal with an existing session, if there is one
    if (send_at("AT+SHSTATE?", "1")) {
        #ifdef DEBUG
        printf("Closing existing HTTP session");
        #endif

        send_at("AT+SHDISC");
    }

    // Configure a session with the server...
    send_at("AT+SHCONF=\"URL\",\"" + server + "\"");

    // ...and open it
    string resp = send_at_response("AT+SHCONN", 2000);

    // The above command may take a while to return, so
    // continue to check the UART until we have a response,
    // or 90s passes (timeout)
    uint32_t now = time_us_32();
    while ((time_us_32() - now) < 90000) {
        if (resp.find("OK") != string::npos) return true;
        if (resp.find("ERROR") != string::npos) return false;
        resp = listen(1000);
    }

    return false;
}

void Sim7080G::end_session() {
    // Break the link to the server
    send_at("AT+SHDISC");

    #ifdef DEBUG
    printf("HTTP session closed");
    #endif
}

/**
    Set a generic request header on the modem.
 */
void Sim7080G::set_request_header() {
    if (!is_header_set) {
        // Clear the header...
        send_at("AT+SHCHEAD");

        // ...and add new header parameters
        send_at("AT+SHAHEAD=\"Content-Type\",\"application/x-www-form-urlencoded\"", "OK", 500);
        send_at("AT+SHAHEAD=\"User-Agent\",\"smittytone-pi-pico/1.0.0\"", "OK", 500);
        send_at("AT+SHAHEAD=\"Cache-control\",\"no-cache\"", "OK", 500);
        send_at("AT+SHAHEAD=\"Connection\",\"keep-alive\"", "OK", 500);
        send_at("AT+SHAHEAD=\"Accept\",\"*/*\"", "OK", 500);

        is_header_set = true;
    }
}

void Sim7080G::set_request_body(string body) {
    send_at("AT+SHCPARA");
    send_at("AT+SHPARA=\"data\",\"" + body + "\"");
}

string Sim7080G::get_data(string server, string path) {
    return issue_request(server, path, "", "GET");
}

string Sim7080G::send_data(string server, string path, string data) {
    return issue_request(server, path, data, "POST");
}

string Sim7080G::issue_request(string server, string path, string body, string verb) {
    string result = "";
    return result;
}

/**
    Make a GET request to the specified server + path.

    - Parameters:
        - server: The target server.
        - path:   The endpoint path.

    - Returns: `true` if the request was successful, otherwise `false`.
 */
bool Sim7080G::request_data(string server, string path) {

    bool success = false;


    return success;
}
