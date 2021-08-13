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
    while (ml_str.size()) {
        int index = ml_str.find("\r");
        if (index != string::npos){
            result.push_back(ml_str.substr(0, index));
            ml_str = ml_str.substr(index + 2);
            if (ml_str.size() == 0) result.push_back(ml_str);
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


}   // namespace Utils