#ifndef MATCH_FUNCTIONS_H
#define MATCH_FUNCTIONS_H

#include <iostream>
#include <string>
#include <vector>

bool match_digit(const std::string& input_line);
bool match_alphanumeric(const std::string input_line);
bool match_positive_group(const std::string& input_line, const std::string& pattern, int start, int end, int& input_pos);
bool match_negative_group(const std::string& input_line, const std::string& pattern, int start, int end, int& input_pos);
bool match_end_anchor(const std::string& input_line, const std::string& pattern);
bool handle_escape_sequence(const std::string& input_line, char escape_char, int& input_pos);
bool handle_repetition(const std::string& input_line, int input_len, char repeat_char, int& input_pos, bool is_escape);
void handle_optional(const std::string& input_line, char pattern_char, int& pattern_index, int& input_pos);
bool handle_wildcard(const std::string& input_line, int& input_pos);
bool handle_alternation(const std::string& input_line, const std::string& pattern, int& pattern_index, int& input_pos);
bool match_backreference(const std::string& input_line, const std::vector<std::string>& capture_groups, int ref_index, int& input_pos);
bool match_combined_character_class(const std::string& input_line, const std::string& pattern);
bool match_pattern(const std::string& input_line, const std::string& pattern);

#endif