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
            printf("Modem ready after %i ms\n", (time_us_32() - start_time) / 1000);
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
    // Set error reporting to 2, set modem to text mode, delete left-over SMS,
    // select LTE-only mode, select Cat-M only mode, set the APN
    send_at("AT+CMEE=2;+CMGF=1;+CMGD=,4;+CNMP=38;+CMNB=1;+CGDCONT=1,\"IP\",\"" + apn + "\"");

    // Set SST version, set SSL no verify, set header config
    send_at("AT+CSSLCFG=\"sslversion\",1,3;+SHSSL=1,\"\";+SHCONF=\"BODYLEN\",1024;+SHCONF=\"HEADERLEN\",350");

    #ifdef DEBUG
    printf("Modem configured for Cat-M and Super SIM\n");
    #endif
}

/**
    Check network connection.
 */
bool Sim7080G::check_network() {

    bool is_connected = false;
    const string response = send_at_response("AT+COPS?");
    const string line = Utils::split_msg(response, 1);
    if (line.find("+COPS:") != string::npos) {
        // ',' will be missing if the modem is not connected,
        // ie. there is no operator value in the AT+COPS? response
        is_connected = (line.find(",") != string::npos);

        #ifdef DEBUG
        if (is_connected) printf("Network information: %s\n", line.c_str());
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
    const string response = send_at_response(cmd, timeout);
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
    const string data_out = cmd + "\r\n";
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
    const uint8_t* buffer_start = &uart_buffer[0];
    rx_ptr = (uint8_t*)buffer_start;

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
    const vector<string> lines = Utils::split_to_lines(msg);
    for (uint32_t i = 0 ; i < lines.size() ; ++i) {
        printf(">>> %s\n", lines[i].c_str());
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
    bool success = false;
    const string response = send_at_response("AT+CNACT?");
    const string line = Utils::split_msg(response, 1);
    const string status = Utils::get_field_value(line, 1);

    if (status == "0") {
        // Inactive data connection so start one up
        success = send_at("AT+CNACT=0,1", "ACTIVE", 2000);
    } else if (status == "1" || status == "2") {
        success = true;
    }

    #ifdef DEBUG
    string base = "Data connection ";
    base += (success ? "active" : "inactive");
    printf("%s\n", base.c_str());
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
    printf("Data connection inactive\n");
    #endif
}

/**
    Open an HTTP connection to the specified server.

    - Parameters:
        - server: The protocol and the server's domain, eg.
                  `https://example.com`
 */
bool Sim7080G::start_session(string server) {
    // Deal with an existing session, if there is one
    if (send_at("AT+SHSTATE?", "1")) {
        #ifdef DEBUG
        printf("Closing existing HTTP session\n");
        #endif

        send_at("AT+SHDISC");
    }

    // Configure a session with the server...
    send_at("AT+SHCONF=\"URL\",\"" + server + "\"");

    // ...and open it
    string response = send_at_response("AT+SHCONN");

    // The above command may take a while to return, so
    // continue to check the UART until we have a response,
    // or `LONG_URC_TIMEOUT` seconds pass (timeout)
    uint32_t now = time_us_32();
    while ((time_us_32() - now) < LONG_URC_TIMEOUT) {
        if (response.find("OK") != string::npos) return true;
        if (response.find("ERROR") != string::npos) return false;
        response = listen(1000);
    }

    return false;
}

/**
    Open an HTTP connection to the specified server.
 */
void Sim7080G::end_session() {
    // Break the link to the server
    send_at("AT+SHDISC");

    #ifdef DEBUG
    printf("HTTP session closed\n");
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
        send_at("AT+SHAHEAD=\"Content-Type\",\"application/x-www-form-urlencoded\";+SHAHEAD=\"User-Agent\",\"smittytone-pi-pico/1.0.0\";+SHAHEAD=\"Cache-control\",\"no-cache\";+SHAHEAD=\"Connection\",\"keep-alive\";+SHAHEAD=\"Accept\",\"*/*\"");
        is_header_set = true;
    }
}

/**
    Clear the modem's internal request body record, and set
    it with the supplied data (as the value of the key `data`).

    - Parameters:
        - body: The data to post.
 */
void Sim7080G::set_request_body(string body) {
    send_at("AT+SHCPARA;+SHPARA=\"data\",\"" + body + "\"");
}

/**
    Make a GET request to the specified server + path.

    The returned data is placed in the instance property `data`.

    - Parameters:
        - server: The target server.
        - path:   The endpoint path.

    - Returns: `true` if the request was successful, otherwise `false`.
 */
bool Sim7080G::get_data(string server, string path) {
    return issue_request(server, path, "", "GET");
}

/**
    Make a POST request to the specified server + path.

    The returned response is placed in the instance property `data`.

    - Parameters:
        - server: The target server.
        - path:   The endpoint path.
        - data:   The data to be posted.

    - Returns: `true` if the request was successful, otherwise `false`.
 */
bool Sim7080G::send_data(string server, string path, string data) {
    return issue_request(server, path, data, "POST");
}

/**
    Make a generic request to the specified server + path.

    The returned response is placed in the instance property `data`.

    - Parameters:
        - server: The target server.
        - path:   The endpoint path.
        - data:   The data to be posted.
        - verb:   The request verb as a string, eg. `"GET"`.

    - Returns: `true` if the request was successful, otherwise `false`.
 */
bool Sim7080G::issue_request(string server, string path, string body, string verb) {
    bool success = false;

    uint32_t code = 1;
    const string verbs[5] = {"GET", "PUT", "POST", "PATCH", "HEAD"};
    verb = Utils::uppercase(verb);
    for (uint32_t i = 0 ; i < 5 ; ++i) {
        if (verb == verbs[i]) {
            code = i + 1;
        }
    }

    if (code < 1 || code > 5) {
        #ifdef DEBUG
        printf("ERROR -- Unknown request verb specified\n");
        #endif

        return false;
    }

    if (start_session(server)) {
        #ifdef DEBUG
        printf("HTTP session open\n");
        #endif

        // Issue the request...
        set_request_header();
        if (body.length() > 0) set_request_body(body);
        string response = send_at_response("AT+SHREQ=\"" + path + "\"," + std::to_string(code));
        uint32_t start = time_us_32();
        while ((time_us_32() - start) < LONG_URC_TIMEOUT) {
            if (response.find("+SHREQ:") != string::npos) break;
            response = listen(1000);
        }

        // ...and process the response
        const vector<string> lines = Utils::split_to_lines(response);
        for (uint32_t i = 0 ; i < lines.size() ; ++i) {
            const string line = lines[i];
            if (line.length() == 0) continue;
            if (line.find("+SHREQ:") != string::npos) {
                const string status_code = Utils::get_field_value(line, 1);
                const string data_length = Utils::get_field_value(line, 2);

                // Break out if we get a bad HTTP status code
                if (std::stoi(status_code) > 299) {
                    #ifdef DEBUG
                    printf("ERROR -- HTTP status code %s\n", status_code.c_str());
                    #endif

                    break;
                }

                if (data_length == "0") break;

                // Get the data from the modem
                response = send_at_response("AT+SHREAD=0," + data_length);

                // The JSON data may be multi-line so store everything in the
                // response that comes after (and including) the first '{'
                uint32_t pos = response.find("{");
                if (pos != string::npos) {
                    data = response.substr(pos);

                    // Only set this if we get what looks like JSON
                    success = true;
                }
            }
        }

        // All done, so Close the session
        end_session();
    } else {
        #ifdef DEBUG
        printf("ERROR -- Could not connect to server %s\n", server.c_str());
        #endif
    }

    return success;
}