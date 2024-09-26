#include <iostream>
#include <string>

bool matchChar(char pattern, char input) {
    switch (pattern) {
        case 'd': return isdigit(input); // matches \d
        case 'w': return isalnum(input);  // matches \w (alphanumeric)
        case 's': return isspace(input);  // matches whitespace
        default: return pattern == input; // matches literal characters
    }
}

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
    if(!pattern.empty()) {
        size_t inputIndex = 0; // Index for the input string
        size_t patternIndex = 0; // Index for the pattern string

        while (inputIndex < input_line.size() && patternIndex < pattern.size()) {
        // If the current pattern character is followed by a number, it indicates repetition
        if (patternIndex + 1 < pattern.size() && isdigit(pattern[patternIndex + 1])) {
            int repeatCount = pattern[patternIndex + 1] - '0'; // Get the repeat count
            while (repeatCount > 0) {
                // Match character class
                if (!matchChar(pattern[patternIndex], input_line[inputIndex])) {
                    return false; // If it doesn't match, return false
                }
                inputIndex++; // Move to the next character in input
                repeatCount--; // Decrement repeat count
                if (inputIndex >= input_line.size()) break; // Avoid out of bounds
            }
            patternIndex += 2; // Move past the character and its count
        } else {
            // Match single character
            if (!matchChar(pattern[patternIndex], input_line[inputIndex])) {
                return false; // If it doesn't match, return false
            }
            inputIndex++; // Move to the next character in input
            patternIndex++; // Move to the next character in pattern
        }
    }

    // Ensure we've consumed all characters in the pattern
    return patternIndex == pattern.size() && inputIndex == input_line.size();
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
