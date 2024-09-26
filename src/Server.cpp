#include <iostream>
#include <string>

bool matchChar(char pattern, char input) {
    switch (pattern) {
        case 'd': return isdigit(input);
        case 'w': return isalnum(input);
        default: return pattern == input;
    }
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if(!pattern.empty()) {
        size_t input_pos = 0;

        while(input_pos < input_line.size()) {
            size_t pattern_pos = 0;
            size_t temp = input_pos;

            while(pattern_pos < pattern.size() && temp < input_line.size()) {
                if(pattern[pattern_pos] == '\\') {
                    ++pattern_pos;

                    if(pattern_pos < pattern.size()) {
                        if(!matchChar(pattern[pattern_pos], input_line[input_pos])) {
                            break;
                        }

                        ++temp;
                    }
                    else {
                        break;
                    }
                }
                else {
                    if(!matchChar(pattern[pattern_pos], input_line[input_pos])) {
                        break;
                    }
                    else {
                        ++temp;
                    }
                }

                ++pattern_pos;
            }

            if(pattern_pos == pattern.size()) return true;
            ++input_pos;
        }
        
        return false;
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
