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
    int pattern_index = 0;
    int input_index = 0;

    while (pattern_index < pattern.size()) {
        char pattern_char = pattern[pattern_index];
        char input_line_char = input_index < input_line.size() ? input_line[input_index] : '\0';  // Handle potential end of input_line

        if (pattern_char == input_line_char) {
            pattern_index++;
            input_index++;
        } else if (pattern_char == '\\') {
            // Handle escape sequences
            pattern_index++;
            pattern_char = pattern[pattern_index];
            if (pattern_char == 'd') {
                // Match a digit
                if (!std::isdigit(input_line_char)) {
                    return false;
                }
            } else if (pattern_char == 'w') {
                // Match a word character
                if (!std::isalnum(input_line_char)) {
                    return false;
                }
            } else if (pattern_char == 's') {
                // Match a whitespace character
                if (!std::isspace(input_line_char)) {
                    return false;
                }
            } else {
                // Invalid escape sequence
                return false;
            }
            pattern_index++;
            input_index++;
        } else {
            // Mismatch or end of pattern but not end of input_line
            return pattern_index == pattern.size() && input_index == input_line.size();
        }
    }

    // If the pattern is exhausted and the input_line is also exhausted, it's a match
    return input_index == input_line.size();
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
