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


namespace Utils {

vector<string> split_to_lines(string str) {
    vector<string> result;
    while (str.size()) {
        int index = str.find("\r");
        if (index != string::npos){
            result.push_back(str.substr(0, index));
            str = str.substr(index + 2);
            if (str.size() == 0) result.push_back(str);
        } else {
            result.push_back(str);
            break;
        }
    }

    return result;
}


/**
    Get a specific line from a multi-line string.

    - Parameters:
        - msg:       The multi-line string
        - want_line: The required line (0 indexed).

    - Returns: The requested line, otherwise an empty string.
 */
string split_msg(string msg, uint32_t want_line) {
    vector<string> lines = split_to_lines(msg);
    for (uint32_t i = 0 ; i < lines.size() ; ++i) {
        if (i == want_line) return lines[i];
    }
    return "";
}


/**
    Get a number from the end of a CMTI line.

    - Parameters:
        - line: The target line.

    - Returns: A pointer to the start of the number, or null
 */
string get_sms_number(string line) {
    uint32_t pos = line.find(",");
    printf("GET_SMS_NUMBER: POS %i\n", pos);
    if (pos == string::npos) return "";
    return line.substr(pos + 1);
}


}