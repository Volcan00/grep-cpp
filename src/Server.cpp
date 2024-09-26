#include <iostream>
#include <string>

bool match_digit(const std::string& input_line) {
    for(size_t i = 0; i < input_line.size(); ++i) {
        if(isdigit(input_line[i]))
            return true;
    }

    return false;
}

bool match_alphanumeric(const std::string input_line) {
    for(size_t i = 0; i < input_line.size(); ++i) {
        if(isalnum(input_line[i]))
            return true;
    }

    return false;
}

bool match_positive_group(const std::string& input_line, const std::string& pattern) {
    std::string char_group = pattern.substr(1, pattern.size() - 2);

    for(char c : char_group) {
        if(input_line.find(c) != std::string::npos)
            return true;
    }

    return false;
}

bool match_negative_group(const std::string& input_string, const std::string& pattern) {
    std::string char_group = pattern.substr(2, pattern.size() - 3);

    for(char c : char_group) {
        if(input_string.find(c) != std::string::npos)
            return false;
    }

    return true;
}

bool match_combined_character_class(const std::string& input_line, const std::string& pattern) {
    size_t input_pos = 0;

    while(input_pos < input_line.size()) {
        size_t pattern_pos = 0;
        size_t input_pos_cpy = input_pos;

        while(input_pos_cpy < input_line.size() && pattern_pos < pattern.size()) {
            if(pattern[pattern_pos] == '\\') {
                ++pattern_pos;

                if(pattern_pos < pattern.size()) {
                    if(pattern[pattern_pos] == 'd') {
                        if(!isdigit(input_line[input_pos_cpy])) {
                            break;
                        }
                    }
                    else {
                        ++input_pos_cpy;
                    }
                }
                else if(pattern[pattern_pos] == 'w') {
                    if(!isalnum(input_line[input_pos_cpy])) {
                        break;
                    }
                    else {
                        ++input_pos_cpy;
                    }
                }
                else
                    break;
            }
            else {
                if(input_line[input_pos_cpy] != pattern[pattern_pos]) {
                    break;
                }
                else {
                    ++input_pos_cpy;
                }
            }
            ++pattern_pos;
        }
        if(pattern_pos == pattern.size()) {
            return true;
        }
        ++input_pos;
    }

    return false;
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if(pattern.size() == 1) {
        return input_line.find(pattern) != std::string::npos;
    }
    else if(pattern[0] == '\\' && pattern.size() == 2) {
        switch (pattern[1])
        {
        case 'd':
            return match_digit(input_line);
            break;
        case 'w':
            return match_alphanumeric(input_line);
            break;
        default:
            throw std::runtime_error("Unhandled pattern " + pattern);
            break;
        }
    }
    else if(pattern.front() =='[' && pattern.back() == ']') {
        bool is_negative_group = (pattern[1] == '^');

        return is_negative_group ? match_negative_group(input_line, pattern) : match_positive_group(input_line, pattern);
    }
    else if(pattern.length() > 2){
        return match_combined_character_class(input_line, pattern);
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
