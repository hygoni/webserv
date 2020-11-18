#ifndef SRCS_TOKENIZE_HPP_
#define SRCS_TOKENIZE_HPP_

/* Copyright 2020 hyeyoo */

#include <string>
#include <vector>

void tokenize(const std::string& str, std::vector<std::string>& token);
void assert_token_size(int size, int minimum);

#endif  // SRCS_TOKENIZE_HPP_
