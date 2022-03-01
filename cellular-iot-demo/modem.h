/*
 * cellular::modem for Raspberry Pi Pico
 *
 * @version     1.0.2
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _MODEM_HEADER_
#define _MODEM_HEADER_

using std::vector;
using std::string;


/*
 * CONSTANTS
 */
#define UART_BUFFER_SIZE        1024
#define PIN_UART_TX             0
#define PIN_UART_RX             1
#define MODEM_UART              uart0

#define SHREQ_DATA_LENGTH_FIELD 2
#define SHREAD_DATA_LINE        4

#define LONG_URC_TIMEOUT        90000000


/**
    A basic driver for the Simcom SIM7080G gobal cellular Cat-M1 modem.
 */
class Sim7080G {

    public:
        // Constructor
        Sim7080G(string network_apn = "super");

        // Methods
        // Modem management
        bool        start_modem();
        bool        check_network();

        // AT command management
        bool            send_at(string cmd, string back = "OK", uint32_t timeout = 500);
        string          send_at_response(string cmd, uint32_t timeout = 500);
        string          listen(uint32_t timeout = 5000);

        // Data connection management
        bool            open_data_conn();
        void            close_data_conn();

        // HTTP session management
        bool            start_session(string server);
        void            end_session();

        // HTTP request management
        bool            get_data(string server, string path);
        bool            send_data(string server, string path, string data);
        bool            issue_request(string server, string path, string body, string verb);
        void            set_request_header();
        void            set_request_body(string body);

        // Properties
        // HTTP request response store
        string data;
    private:
        // Methods
        // Modem management
        bool            boot_modem();
        void            config_modem();
        void            toggle_module_power();

        // Misc.
        void            debug_output(string msg);
        void            read_buffer(uint32_t timeout = 5000);
        void            clear_buffer();
        string          buffer_to_string();

        // Properties
        uint8_t         uart_buffer[UART_BUFFER_SIZE];
        uint8_t         *rx_ptr;
        string          apn;
        bool            is_header_set;
};


#endif // _MODEM_HEADER_
