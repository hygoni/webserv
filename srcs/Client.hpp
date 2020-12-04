#ifndef SRCS_CLIENT_HPP_
#define SRCS_CLIENT_HPP_

#include "Request.hpp"
#include "Location.hpp"
#include <netinet/in.h>

class Response;
class Client {
private:
  int         _fd;
  Request*    _request;
  Response*   _response;
  std::string _raw_request;
  const std::vector<Location>& _locations;

public:
  Client(int server_fd, const std::vector<Location>& locations);
  ~Client();

  int   recv();
  int   getFd() const;
};

#endif  //  SRCS_CLIENT_HPP_
