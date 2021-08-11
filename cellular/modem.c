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
bool send_at(char* cmd, char* back, uint32_t timeout) {
    uint32_t length = send_at_response(cmd, timeout);
    return (length > 0 && strstr(&uart_buffer[0], back));
}


/**
    Send an AT command to the modem.

    - Parameters:
        - cmd:     pointer to the command string.
        - timeout: milliseconds to wait for response data.

    - Returns: The number of bytes received.
 */
uint32_t send_at_response(char* cmd, uint32_t timeout) {
    // Write out the AT command
    char* send = strcat(cmd, "\r\n");
    uart_write_blocking(MODEM_UART, send, strlen(send));

    // Read the response
    char* buffer_start = &uart_buffer[0];
    rx_ptr = buffer_start;
    uint32_t now = time_us_32();
    while (time_us_32() - now < timeout || rx_ptr - buffer_start >= UART_BUFFER_SIZE) {
        if (uart_is_readable(MODEM_UART) > 0) {
            uart_read_blocking(MODEM_UART, rx_ptr, 1);
        }
    }

    // Return number of bytes read
    return rx_ptr - buffer_start;
}


/**
    Clear the RX buffer with zeroes.
 */
void clear_buffer() {
    for (uint32_t i = 0 ; i < 256 ; ++i) {
        uart_buffer[i] = 0;
    }
}


/**
    Initialise the modem.

    - Returns: `true` if the modem is ready, otherwise `false`.
 */
bool init_modem() {
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
 */
bool start_modem() {
    bool state = false;

    for (uint32_t i = 0 ; i < 10 ; ++i) {
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
    APN for Super SIM usage
 */
void init_network(char* apn) {
    // Set error reporting to 2
    send_at("AT+CMEE=2", "OK", 2000);

    // Set modem to text mode
    send_at("AT+CMGF=1", "OK", 2000);

    // Select LTE-only mode
    send_at("AT+CNMP=38", "OK", 2000);

    // Select Cat-M only mode
    send_at("AT+CMNB=1", "OK", 2000);

    // Set the APN
    char apn_cmd[strlen(apn) + 20];
    strcpy(apn_cmd, "AT+CGDCONT=1,\"IP\",\"");
    strcat(apn_cmd, apn);
    strcat(apn_cmd, "\"");
    send_at(apn_cmd, "OK", 2000);
}


/**
    Toggle the modem power line.
 */
void toggle_module_power() {
    // Power the pin
    gpio_put(PIN_MODEM_PWR, true);

    // Wait at least 1.5 seconds
    sleep_ms(1500);

    // Ground the pin
    gpio_put(PIN_MODEM_PWR, true);
}


void split_msg(char* msg, char **ptrs, int32_t want_line) {

    uint32_t count = 0;
    char* str_ptr = msg;
    ptrs[count] = msg;
    count++;
    char tmp[3];

    for (uint32_t i = 0 ; i < strlen(msg) ; ++i) {
        strncpy(tmp, str_ptr, 2);
        if (strstr(tmp, "\r\n") != 0) {
            ptrs[count] = str_ptr;
            count++;
            str_ptr++;
        }

        str_ptr++;
    }

    if (count < 20) {
        for (uint32_t i = count ; i < 20 ; ++i) {
            ptrs[i] = NULL;
        }
    }
}


char* get_line(char* msg, int32_t want_line) {
    uint32_t line_number = 0;
    const char token[2] = "\r";
    char *next_ptr;
    char *start_ptr = msg;

    // Get first token
    next_ptr = strtok(msg, token);
    if (next_ptr) {
        while (next_ptr) {
            if (strcmp(next_ptr + 1, "\n") == 0) {
                // Is this the line we want? If so, return a pointer to it
                if (line_number == want_line) return start_ptr;

                // Move to start of next line
                next_ptr++;
                start_ptr = next_ptr;
            }

            // Get next token
            next_ptr = strtok(NULL, token);
        }
    }

    return NULL;
}


char* get_sms_number(char* line) {
    char* p = strstr(line, ",");
    if (p) return p + 1;
    return NULL;
}