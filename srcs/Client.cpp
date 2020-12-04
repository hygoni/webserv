#include "Client.hpp"
#include "Response.hpp"
#include "Fd.hpp"
#define MAX_HEADER_SIZE 8192

Client::Client(int server_fd, const std::vector<Location>& locations) : _locations(locations) {
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

  /* buffer is not empty yet */
  if (_response != NULL && response->isBodyReady())
    return 0;

  if ((n_read = ::recv(_fd, buf, BUFSIZE - 1, 0)) > 0) {
    buf[n_read] = '\0';
    std::cout << buf << std::endl;
    _raw_request += buf;
    if (_request == NULL) {
      header_end = _raw_request.find("\r\n\r\n");
      if (header_end == std::string::npos) {
        if (_raw_request.size() <= MAX_HEADER_SIZE) {
          return (0);
        }
        std::cout << "header oversize" << std::end;
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

          /* make new response */
          if (_response == NULL) {
            _response = new Response(*_request, _locations);
            _response->setBodyWriteFd(_fd);
          }

          /* save data to buffer */
//          response->putBodybuffer(buf, n_read);

          if (_request->isClosed()) {
            //_response->closeBody(_fd);
          }
        } else {
          // header oversize
          return (1);          
        }
      }
    } else {
      // add body
      
    }
  } else {
    throw std::exception();
  }
  return (1);
}

int   Client::getFd() const {
  return _fd;
}
