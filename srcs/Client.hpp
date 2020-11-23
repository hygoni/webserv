#ifndef SRCS_CLIENT_HPP_
#define SRCS_CLIENT_HPP_

#include "Request.hpp"
#include "Socket.hpp"
#include <netinet/in.h>

class Response;
class Client : public Socket {
private:
  int        _fd;
  Request*   _request;
  Response*  _response;

  Client();
public:
  Client(int server_fd);
  ~Client();

  int   getFd() const;
};

#endif  //  SRCS_CLIENT_HPP_
