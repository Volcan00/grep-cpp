#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

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

bool match_positive_group(const std::string& input_line, const std::string& pattern, int start, int end, int& input_pos) {
    bool matched = false;
    while (input_pos < input_line.size()) {
        bool found = false;
        for (int idx = start + 1; idx < end; ++idx) {
            if (input_line[input_pos] == pattern[idx]) {
                ++input_pos;
                found = true;
                matched = true;
                break;
            }
        }
        if (!found) break;
    }
    return matched;
}

bool match_negative_group(const std::string& input_line, const std::string& pattern, int start, int end, int& input_pos) {
    if (input_pos < input_line.size()) {
        char input_char = input_line[input_pos];
        for (int idx = start + 1; idx < end; ++idx) {
            if (input_char == pattern[idx]) {
                return false;
            }
        }
        ++input_pos;
        return true;
    }
    return false;
}


bool match_end_anchor(const std::string& input_line, const std::string& pattern) {
    return input_line == pattern;
}

bool handle_escape_sequence(const std::string& input_line, char escape_char, int& input_pos) {
    switch (escape_char)
    {
        case 'd':
            if(std::isdigit(input_line[input_pos])) {
                ++input_pos;
                return true;
            }
            break;
        case 'w':
            if(std::isalnum(input_line[input_pos])) {
                ++input_pos;
                return true;
            }
            break;
        default:
            return false;
    }
    return false;
}

bool handle_repetition(const std::string& input_line, int input_len, char repeat_char, int& input_pos, bool is_escape) {
    if (is_escape) {
        if (repeat_char == 'd') {
            while (input_pos < input_len && std::isdigit(input_line[input_pos])) {
                ++input_pos;
            }
        } else if (repeat_char == 'w') {
            while (input_pos < input_len && std::isalnum(input_line[input_pos])) {
                ++input_pos;
            }
        } else {
            return false;
        }
    } else {
        char prev_char = input_line[input_pos - 1];
        if (prev_char != repeat_char) {
            return false;
        }

        while (input_pos < input_len && input_line[input_pos] == repeat_char) {
            ++input_pos;
        }
    }

    return true;
}

void handle_optional(const std::string& input_line, char pattern_char, int& pattern_index, int& input_pos) {
    if (input_line[input_pos] == pattern_char) {
        ++input_pos;
    }
    
    ++pattern_index;
}

bool handle_wildcard(const std::string& input_line, int& input_pos) {
    if(std::isalpha(input_line[input_pos])) {
        ++input_pos;
        return true;
    }

    ++input_pos;

    return false;
}

bool match_combined_character_class(const std::string& input_line, const std::string& pattern) {
    int pattern_len = pattern.size();
    int input_len = input_line.size();
    int pattern_pos = 0;
    bool optional = pattern.find('?') != std::string::npos;
    bool endAnchor = (pattern_len > 0 && pattern[pattern_len - 1] == '$');

    if(endAnchor) {
        --pattern_len;
    }

    for(int input_index = 0; input_index < input_len; ++input_index) {
        int input_pos = input_index;
        int pattern_index = 0;

        bool startAnchor = false;

        while(pattern_index < pattern_len && input_pos < input_len) {
            char pattern_char = pattern[pattern_index];
            bool is_escape = false;

            if(pattern_char == '^') {
            ++pattern_index;
            startAnchor = true;
            continue;
            }
            else if(pattern_char == '\\') {
                ++pattern_index;

                if(pattern_index >= pattern_len){
                    return false;
                }

                char escape_char = pattern[pattern_index];
                is_escape = true;
                if(!handle_escape_sequence(input_line, escape_char, input_pos)) {
                    break;
                }
                
                if(pattern_pos + 1 < pattern_len && pattern[pattern_index + 1] == '+') {
                    if(!handle_repetition(input_line, input_len, pattern[pattern_index], input_pos, is_escape)) {
                        return false;
                    }

                    pattern_index += 2;
                    continue;
                }
            }
            else if (pattern_char == '[') {
                int begin = pattern_index;
                while (pattern_index < pattern_len && pattern[pattern_index] != ']') {
                    ++pattern_index;
                }

                if (pattern_index >= pattern_len) {
                    return false;
                }

                bool is_negative = (pattern[begin + 1] == '^');
                bool matched = false;

                if (is_negative) {
                    matched = match_negative_group(input_line, pattern, begin, pattern_index, input_pos);
                } else {
                    matched = match_positive_group(input_line, pattern, begin, pattern_index, input_pos);
                }

                if (!matched) {
                    break;
                }

                if (pattern_index + 1 < pattern_len && pattern[pattern_index + 1] == '+') {
                    if (!matched) return false; 
                    ++pattern_index;
                }

            }
            else if(pattern_char == '+') {
                if(!handle_repetition(input_line, input_len, pattern[pattern_index - 1], input_pos, is_escape)) {
                    return false;
                }
            }
            else if (pattern_index + 1 < pattern_len && pattern[pattern_index + 1] == '?') {
                handle_optional(input_line, pattern_char, pattern_index, input_pos);
            }
            else if(pattern_char == '.') {
                if(!handle_wildcard(input_line, input_pos)) {
                    break;
                }
            }
            else {
                if(input_line[input_pos] != pattern_char) {
                    break;
                }
                
                ++input_pos;
            }

            ++pattern_index;
            pattern_pos = pattern_index;
        }

        if(pattern_index == pattern_len) {
            if(endAnchor) {
                return input_pos == input_len;
            }

            return true;
        }

        if(startAnchor && pattern_index != pattern_len) {
            return false;
        }
    }

    if(pattern[pattern_len - 1] == '+' && pattern_pos == pattern_len - 1) {
        return true;
    }

    if (optional && pattern[pattern_len - 1] == '?' && pattern_pos == pattern_len - 2) {
        return true;
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
    else if(pattern.find('(') != std::string::npos && pattern.find(')') != std::string::npos) {
        int openParenPos = pattern.find('(');
        int closeParenPos = pattern.find(')');

        if(pattern.find('|') != std::string::npos) {
            int pipePos = pattern.find('|');

            std::string sub_pattern_1 = pattern.substr(0, openParenPos + 1) + pattern.substr(openParenPos + 1, pipePos - openParenPos - 1) + ")" + pattern.substr(closeParenPos + 1);
            std::string sub_pattern_2 = pattern.substr(0, openParenPos + 1) + pattern.substr(pipePos + 1, closeParenPos - pipePos) + pattern.substr(closeParenPos + 1);

            return (match_pattern(input_line, sub_pattern_1) || match_pattern(input_line, sub_pattern_2));
        }
        else {
            std::string new_pattern = "";
            std::string capturedGroup = pattern.substr(openParenPos + 1, closeParenPos - openParenPos - 1);

            for(size_t i = 0; i < pattern.size(); ++i) {
                if(pattern[i] == '(' || pattern[i] == ')') {
                    continue;
                }

                if(std::isdigit(pattern[i]) && i > 0 && pattern[i - 1] == '\\') {
                    new_pattern.pop_back();
                    new_pattern += capturedGroup;
                }
                else {
                    new_pattern += pattern[i];
                }
            }

            return match_pattern(input_line, new_pattern);
        }
    }
    else if(pattern.length() > 1) {
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
            std::cout << "Matched" << '\n';
            return 0;
        } else {
            std::cout << "Not matched" << '\n';
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
