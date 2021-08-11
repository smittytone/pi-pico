/*
 * cellular for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _MODEM_HEADER_
#define _MODEM_HEADER_


/*
 * PROTOTYPES
 */
bool        send_at(char* cmd, char* back, uint32_t timeout);
uint32_t    send_at_response(char* cmd, uint32_t timeout);
void        clear_buffer();

bool        init_modem();
bool        start_modem();
void        init_network(char* apn);
void        toggle_module_power();

void        split_msg(char* msg, char **ptrs, int32_t want_line);
char*       get_line(char* msg, int32_t want_line);
char*       get_sms_number(char* line);


#endif // _MODEM_HEADER_
