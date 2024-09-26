#include <iostream>
#include <string>

bool matchChar(char pattern, char input) {
    switch (pattern) {
        case 'd': return isdigit(input); // matches \d
        case 'w': return isalnum(input);  // matches \w (alphanumeric)
        default: return pattern == input; // matches literal characters
    }
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if(!pattern.empty()) {
        size_t inputIndex = 0; // Index for the input string
        size_t patternIndex = 0; // Index for the pattern string

        while (patternIndex < pattern.size()) {
            if (pattern[patternIndex] == '\\') {
                // If the pattern has an escape character
                patternIndex++; // Move to the next character (d, w)
                if (patternIndex >= pattern.size()) return false; // Invalid pattern
                char currentPattern = pattern[patternIndex];
                // Ensure there are still characters in the input to match against
                if (inputIndex >= input_line.size() || !matchChar(currentPattern, input_line[inputIndex])) {
                    return false; // If it doesn't match, return false
                }
                inputIndex++; // Move to the next character in input
                patternIndex++; // Move to the next character in pattern
            } else {
                // Match a literal character (including spaces)
                if (inputIndex >= input_line.size() || !matchChar(pattern[patternIndex], input_line[inputIndex])) {
                    return false; // If it doesn't match, return false
                }
                inputIndex++; // Move to the next character in input
                patternIndex++; // Move to the next character in pattern
            }
        }

    // Ensure we've consumed all characters in the pattern and input
        return inputIndex == input_line.size();
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
