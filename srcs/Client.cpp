#include "Client.hpp"
#include "Response.hpp"

Client::Client() {
}

Client::Client(int server_fd) {
  socklen_t     addr_len;
  sockaddr_in   client_addr;

  addr_len = sizeof(client_addr);
  _fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
  if (_fd < 0)
    throw std::exception();
  _request = NULL;
  _response = NULL;
}

Client::~Client() {
  if (_request != NULL)
    delete _request;
  if (_response != NULL)
    delete _response;
}

int   Client::getFd() const {
  return _fd;
}
