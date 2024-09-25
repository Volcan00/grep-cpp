#include <iostream>
#include <string>

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    }
    else if(pattern == "\\d") {
        return input_line.find_first_of("01234567890") != std::string::npos;
    }
    else if(pattern == "\\w") {
        for(char c : input_line)
        {
            if(!isalnum(c))
                return false;
        }

        return true;
    }
    else if(pattern.front() =='[' && pattern.back() == ']') {
        std::string char_group = pattern.substr(1, pattern.length() - 2);

        for(char c : char_group) {
            if(input_line.find(c) != std::string::npos)
                return true;
        }

        return false;
    }
    else if(pattern.front() == '[' && pattern.at(1) == '^' && pattern.back() == ']') {
        std::string char_group = pattern.substr(1, pattern.length() - 2);

        for(char c : char_group) {
            if(input_line.find(c) != std::string::npos)
                return false;
        }

        return true;
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
