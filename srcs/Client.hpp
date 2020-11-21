#ifndef SRCS_CLIENT_HPP_
#define SRCS_CLIENT_HPP_

#include "Request.hpp"
#include "Response.hpp"

class Client {
 private:
   int        _fd;
   Request*   _request;
   Response*  _response;
 public:

}

#endif  //  SRCS_CLIENT_HPP_
