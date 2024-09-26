#include <iostream>
#include <string>

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    // if (pattern.length() == 1) {
    //     return input_line.find(pattern) != std::string::npos;
    // }
    // else if(pattern == "\\d") {
    //     return input_line.find_first_of("01234567890") != std::string::npos;
    // }
    // else if(pattern == "\\w") {
    //     for(char c : input_line)
    //     {
    //         if(!isalnum(c))
    //             return false;
    //     }

    //     return true;
    // }
    // if(!pattern.empty()) {
    //     size_t pattern_pos = 0;
    //     size_t input_pos = 0;

    //     while(pattern_pos < pattern.size() && input_pos < input_line.size()) {
    //         if(pattern[pattern_pos] == '\\') {
    //             if(pattern_pos + 1 < pattern.size()) {
    //                 char special_character = pattern[pattern_pos + 1];

    //                 if(special_character == 'd') {
    //                     if(!isdigit(input_line[input_pos]))
    //                         return false;
    //                 }

    //                 ++input_pos;
    //                 pattern_pos += 2;

    //             }
    //         }
    //         else {
    //             if(pattern[pattern_pos != input_line[input_pos]])
    //                 return false;

    //             ++input_pos;
    //             ++pattern_pos;
    //         }
    //     }

    //     return (pattern_pos == pattern.size());
    // }
    if(!pattern.empty()) {
        size_t patter_pos = 0;
        size_t input_pos = 0;

        while(patter_pos < pattern.length() && input_pos < input_line.length()) {
            if(pattern[patter_pos] == '\\') {
                if(patter_pos + 1 < pattern.length()) {
                    char special_character = pattern[patter_pos + 1];

                    if(special_character == 'd') {
                        while(input_pos < input_line.length() && !isdigit(input_line[input_pos])) {
                            ++input_pos;
                        }

                        if(input_pos >= input_line.length()) {
                            return false;
                        }

                        ++input_pos;
                    }
                    else if(special_character == 'w') {
                        if(!isalnum(input_line[input_pos])) {
                            return false;
                        }
                        ++input_pos;
                    }

                    patter_pos += 2;
                }
            }
            else if(isalpha(pattern[patter_pos])) {
                if(pattern[patter_pos] != input_line[input_pos]) {
                    return false;
                }

                ++input_pos;
                ++patter_pos;
            }
        }

        return true;
    }
    else if(pattern.front() =='[' && pattern.back() == ']') {
        bool is_negative_group = (pattern[1] == '^');

        std::string char_group = is_negative_group
                                    ? pattern.substr(2, pattern.length() - 3)
                                    : pattern.substr(1, pattern.length() - 2);

        if(is_negative_group) {
            for(char c : char_group) {
                if(input_line.find(c) != std::string::npos)
                    return false;
            }

            return true;
        }
        else {
            for(char c : char_group) {
                if(input_line.find(c) != std::string::npos)
                    return true;
            }

            return false;
        }
    }
    else {
        throw std::runtime_error("Unhandled pattern " + pattern);
    }
}

int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];


    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }
    
    std::string input_line;
    std::getline(std::cin, input_line);
    
    try {
        if (match_pattern(input_line, pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
