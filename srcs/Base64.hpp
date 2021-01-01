#ifndef SRCS_BASE64_HPP_
#define SRCS_BASE64_HPP_

#include <string>

class Base64 {
 public:
    static std::string Encode(std::string const& data);
    static std::string Decode(std::string const& input);
};
#endif  // SRCS_BASE64_HPP_