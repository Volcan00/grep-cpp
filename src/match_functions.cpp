#include "../src/MatchFunctions.h"

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

bool handle_character_group(const std::string& input_line, const std::string& pattern, int& pattern_index, int& input_pos, int pattern_len) {
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
        return false;
    }

    if (pattern_index + 1 < pattern_len && pattern[pattern_index + 1] == '+') {
        if (!matched) return false; 
        ++pattern_index;
    }

    return true;
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

bool handle_alternation(const std::string& input_line, const std::string& pattern, int& pattern_index, int& input_pos) {
    size_t group_end = pattern.find(')', pattern_index);
    if (group_end == std::string::npos) {
        return false; // No closing parenthesis
    }

    size_t pipe_pos = pattern.find('|', pattern_index);
    if (pipe_pos != std::string::npos && pipe_pos < group_end) {
        // Capture the left and right patterns
        std::string left_pattern = pattern.substr(pattern_index + 1, pipe_pos - pattern_index - 1);
        std::string right_pattern = pattern.substr(pipe_pos + 1, group_end - pipe_pos - 1);

        int original_pos = input_pos; // Store the current input position
        if (match_combined_character_class(input_line.substr(input_pos), left_pattern)) {
            input_pos += left_pattern.length();
            pattern_index = group_end + 1;
            return true;
        }

        // If left pattern fails, try right pattern
        input_pos = original_pos; // Reset input position to try right pattern
        if (match_combined_character_class(input_line.substr(input_pos), right_pattern)) {
            input_pos += right_pattern.length();
            pattern_index = group_end + 1;
            return true;
        }

        return false; // Both patterns failed
    }

    return false; // No alternation found
}


bool match_combined_character_class(const std::string& input_line, const std::string& pattern) {
    int pattern_len = pattern.size();
    int input_len = input_line.size();
    int pattern_pos = 0;
    bool endAnchor = (pattern_len > 0 && pattern[pattern_len - 1] == '$');

    if(endAnchor) {
        --pattern_len;
    }

    std::vector<std::string> capture_groups;

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

                char next_char = pattern[pattern_index];

                is_escape = true;
                if(!handle_escape_sequence(input_line, next_char, input_pos)) {
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
                if(!handle_character_group(input_line, pattern, pattern_index, input_pos, pattern_len)) {
                    break;
                }
            }
            else if (pattern_char == '(') {
                    if (!handle_alternation(input_line, pattern, pattern_index, input_pos)) {
                        return false;
                    }
                    else {
                        continue;
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

    if (pattern[pattern_len - 1] == '?' && pattern_pos == pattern_len - 2) {
        return true;
    }

    return false;
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if(pattern.size() == 1) {
        return input_line.find(pattern) != std::string::npos;
    }
    else if(pattern.length() > 1) {
        return match_combined_character_class(input_line, pattern);
    }
    else {
        throw std::runtime_error("Unhandled pattern " + pattern);
    }
}