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
    int input_len = input_line.size();
    int pat_len = pattern.size();
    
    int input_pos = 0;
    int pattern_pos = 0;
    
    // Traverse both the input_line and the pattern
    while (input_pos < input_len && pattern_pos < pat_len) {
        if (pattern[pattern_pos] == '\\') {
            // Check the next character after the backslash
            if (pattern_pos + 1 >= pat_len) return false; // Invalid pattern
            char nextChar = pattern[pattern_pos + 1];
            
            if (nextChar == 'd') {
                // \d matches any digit
                if (!std::isdigit(input_line[input_pos])) return false;
            } else if (nextChar == 'w') {
                // \w matches any word character (alphanumeric or _)
                if (!std::isalnum(input_line[input_pos])) return false;
            } else {
                // Invalid escape sequence, return false
                return false;
            }
            
            // Move to the next character in the pattern
            pattern_pos += 2;
        } else {
            // Literal character match
            if (pattern[pattern_pos] != input_line[input_pos]) {
                return false;
            }
            pattern_pos++;
        }
        input_pos++;
    }
    
    // If the pattern is fully consumed and the input_line is fully matched, it's a success
    return pattern_pos == pat_len && input_pos == input_len;
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
