/*
 * cellular::utils for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_


namespace Utils {
    std::vector<string>     split_to_lines(string str);
    string                  split_msg(string msg, uint32_t want_line);
    string                  get_sms_number(string line);
}


#endif // _UTILS_HEADER_