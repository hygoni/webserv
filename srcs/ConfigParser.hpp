#ifndef SRCS_CONFIGPARSER_HPP_
#define SRCS_CONFIGPARSER_HPP_

/* Copyright 2020 hyeyoo */

#include "Context.hpp"

class ConfigParser {
 public:
    static bool parseKey(Context& ctx);
    static bool parseValue(Context& ctx);
    static bool parseAttribute(Context& ctx);
    static bool parseLocation(Context& ctx);
};

#endif  // SRCS_CONFIGPARSER_HPP_
