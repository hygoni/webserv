#include "Client.hpp"
#include "Response.hpp"
#define MAX_HEADER_SIZE 8192

Client::Client() {
}

Client::Client(int server_fd) {
  socklen_t     addr_len;
  sockaddr_in   client_addr;

  addr_len = sizeof(client_addr);
  _fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
  if (_fd < 0)
    throw std::exception();
  _raw_request = std::string();
  _request = NULL;
  _response = NULL;
}

Client::~Client() {
  if (_request != NULL)
    delete _request;
  if (_response != NULL)
    delete _response;
}

int  Client::recv() {
  char    buf[BUFSIZE];
  int     n_read;
  size_t  header_end;

  if ((n_read = ::recv(this->_fd, buf, BUFSIZE - 1, 0)) > 0) {
    buf[n_read] = '\0';
    _raw_request += buf;
    if (_request == NULL) {
      header_end = _raw_request.find("\r\n\r\n");
      if (header_end == std::string::npos) {
        if (_raw_request.size() <= MAX_HEADER_SIZE) {
          return (0);
        }
        // header oversize
        return (1);
      } else {
        if (header_end <= MAX_HEADER_SIZE) {
          try {
            _request = new Request(_raw_request);
          } catch (HttpException & err) {
            std::cout << "exception:" << err.getStatus() << std::endl;
            Response response(err.getStatus());
            response.send(_fd);
            return (1);
          }
          if (_request->isClosed()) {
            
          }
        } else {
          // header oversize
          return (1);          
        }
      }
    }
  } else {
    throw std::exception();
  }
}

int   Client::getFd() const {
  return _fd;
}
