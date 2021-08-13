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

using std::string;


#define UART_BUFFER_SIZE        256
#define PIN_UART_TX             0
#define PIN_UART_RX             1
#define MODEM_UART              uart0


class Sim7080G {

    public:
        Sim7080G(string network_apn);

        bool        send_at(string cmd, string back, uint32_t timeout);
        string      send_at_response(string cmd, uint32_t timeout);
        void        read_buffer(uint32_t timeout);
        void        clear_buffer();
        string      buffer_to_string();

        bool        start_modem();
        bool        init_modem();
        void        init_network();
        void        toggle_module_power();

        string      listen(uint32_t timeout);

    private:
        uint8_t     uart_buffer[UART_BUFFER_SIZE];
        uint8_t     *rx_ptr;
        string      apn;
};


#endif // _MODEM_HEADER_
