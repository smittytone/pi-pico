/*
 * cellular::utils for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "cellular.h"

using std::vector;
using std::string;


namespace Utils {

/**
    Convert a multi-line string into an array of lines,
    split at `\r\n` - as issued by the modem.

    - Parameters:
        - ml_str: The multi-line string

    - Returns: The requested line, otherwise an empty string.
 */
vector<string> split_to_lines(string ml_str) {
    vector<string> result;
    while (ml_str.length()) {
        int index = ml_str.find("\r\n");
        if (index != string::npos){
            result.push_back(ml_str.substr(0, index));
            ml_str = ml_str.substr(index + 2);
            //if (ml_str.size() == 0) result.push_back(ml_str);
        } else {
            result.push_back(ml_str);
            break;
        }
    }

    return result;
}

/**
    Get a specific line from a multi-line string.

    - Parameters:
        - ml_str:    The multi-line string
        - want_line: The required line (0 indexed).

    - Returns: The requested line, otherwise an empty string.
 */
string split_msg(string ml_str, uint32_t want_line) {
    vector<string> lines = split_to_lines(ml_str);
    for (uint32_t i = 0 ; i < lines.size() ; ++i) {
        if (i == want_line) return lines[i];
    }
    return "";
}

/**
    Get a number from the end of a +CMTI line from the modem.

    - Parameters:
        - line: The target line.

    - Returns: A pointer to the start of the number, or null
 */
string get_sms_number(string line) {
    uint32_t pos = line.find(",");

    #ifdef DEBUG
    printf("GET_SMS_NUMBER: POS %i\n", pos);
    #endif

    if (pos != string::npos) return line.substr(pos + 1);
    return "";
}


/**
    Convert a 16-bit int (to cover decimal range 0-9999) to
    its BCD equivalent.

    - Parameters:
        - base: The input integer.

    - Returns: The BCD encoding of the input.
 */
uint32_t bcd(uint32_t base) {
    if (base > 9999) base = 9999;
    for (uint32_t i = 0 ; i < 16 ; ++i) {
        base = base << 1;
        if (i == 15) break;
        if ((base & 0x000F0000) > 0x0004FFFF) base += 0x00030000;
        if ((base & 0x00F00000) > 0x004FFFFF) base += 0x00300000;
        if ((base & 0x0F000000) > 0x04FFFFFF) base += 0x03000000;
        if ((base & 0xF0000000) > 0x4FFFFFFF) base += 0x30000000;
    }

    return (base >> 16) & 0xFFFF;
}


}   // namespace Utils