/*
 * cellular::modem for Raspberry Pi Pico
 *
 * @version     1.0.1
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _MODEM_HEADER_
#define _MODEM_HEADER_


/*
 * CONSTANTS
 */
#define UART_BUFFER_SIZE        256
#define PIN_UART_TX             0
#define PIN_UART_RX             1
#define MODEM_UART              uart0

#define SHREQ_DATA_LENGTH_FIELD 2
#define SHREAD_DATA_LINE        4


/**
    A basic driver for the Simcom SIM7080G gobal cellular Cat-M1 modem.
 */
class Sim7080G {

    public:
        // Constructor
        Sim7080G(std::string network_apn = "super");

        // Methods
        bool        send_at(std::string cmd, std::string back = "OK", uint32_t timeout = 1000);
        std::string send_at_response(std::string cmd, uint32_t timeout = 2000);
        void        read_buffer(uint32_t timeout = 5000);
        std::string buffer_to_string();

        bool        start_modem();
        bool        boot_modem();
        void        config_modem();
        bool        check_network();

        std::string listen(uint32_t timeout = 5000);

        bool        open_data_conn();
        void        close_data_conn();

        bool        start_session(std::string server);
        void        end_session();

        void        set_request_header();
        void        set_request_body(std::string body);

        std::string get_data(std::string server, std::string path);
        std::string send_data(std::string server, std::string path, std::string data);
        std::string issue_request(std::string server, std::string path, std::string body, std::string verb);
        bool        request_data(std::string server, std::string path);



        // Properties
        std::string data;
    private:
        // Methods
        void        debug_output(std::string msg);
        void        toggle_module_power();
        void        clear_buffer();

        // Properties
        uint8_t     uart_buffer[UART_BUFFER_SIZE];
        uint8_t     *rx_ptr;
        std::string apn;
        bool        is_header_set;
};


#endif // _MODEM_HEADER_
