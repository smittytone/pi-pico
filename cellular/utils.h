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


/*
 * PROTOTYPES
 */
namespace Utils {
    std::vector<std::string>    split_to_lines(std::string str, std::string sep = "\r\n");
    std::string                 split_msg(std::string msg, uint32_t want_line);
    std::string                 get_sms_number(std::string line);
    uint32_t                    bcd(uint32_t base);
}


#endif // _UTILS_HEADER_