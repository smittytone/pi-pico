/*
 * cellular::modem for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _MODEM_HEADER_
#define _MODEM_HEADER_


#define UART_BUFFER_SIZE        256
#define PIN_UART_TX             0
#define PIN_UART_RX             1
#define MODEM_UART              uart0


class Sim7080G {

    public:
        Sim7080G(std::string network_apn = "super");

        bool        send_at(std::string cmd, std::string back = "OK", uint32_t timeout = 2000);
        std::string send_at_response(std::string cmd, uint32_t timeout = 2000);
        void        read_buffer(uint32_t timeout = 5000);
        void        clear_buffer();
        std::string buffer_to_string();

        bool        start_modem();
        bool        boot_modem();
        void        config_modem();
        void        toggle_module_power();
        bool        check_network();

        std::string listen(uint32_t timeout = 5000);

    private:
        uint8_t     uart_buffer[UART_BUFFER_SIZE];
        uint8_t     *rx_ptr;
        std::string apn;
};


#endif // _MODEM_HEADER_
