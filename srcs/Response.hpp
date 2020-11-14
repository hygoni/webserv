#ifndef SRCS_RESPONSE_HPP_
#define SRCS_RESPONSE_HPP_

/* Copyright 2020 hyeyoo */

#include "Request.hpp"

class Response {
 public:
    explicit  Response(Request const& req);
    void      send(int fd);
};

#endif  // SRCS_RESPONSE_HPP_

