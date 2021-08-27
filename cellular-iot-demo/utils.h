/*
 * cellular::utils for Raspberry Pi Pico
 *
 * @version     1.0.1
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_

using std::vector;
using std::string;


/*
 * PROTOTYPES
 */
namespace Utils {
    vector<string>  split_to_lines(string str, string sep = "\r\n");
    string          split_msg(string msg, uint32_t want_line);
    string          get_sms_number(string line);
    string          get_field_value(string line, uint32_t field_number);
    uint32_t        bcd(uint32_t base);
    string          uppercase(string base);
}


#endif // _UTILS_HEADER_