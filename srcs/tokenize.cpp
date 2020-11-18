/* Copyright 2020 hyeyoo */

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "tokenize.hpp"

void tokenize(const std::string& str, std::vector<std::string>& token) {
  const std::string delimiter = " \t";
  std::string::size_type last_pos = str.find_first_not_of(delimiter, 0);
  std::string::size_type pos = str.find_first_of(delimiter, 0);

  while (std::string::npos != pos || std::string::npos != last_pos) {
    token.push_back(str.substr(last_pos, pos - last_pos));
    last_pos = str.find_first_not_of(delimiter, pos);
    pos = str.find_first_of(delimiter, last_pos);
  }
}

void assert_token_size(int size, int minimum) {
    if (minimum == 0)
        return;
    if (size < minimum)
        throw std::exception();
}
