#include "Client.hpp"
#include "Response.hpp"
#include "Fd.hpp"
#define MAX_HEADER_SIZE 8192

Client::Client(Server const& server) : _server(server) {
  socklen_t     addr_len;
  sockaddr_in   client_addr;

  addr_len = sizeof(client_addr);
  _fd = accept(server.getFd(), (struct sockaddr *)&client_addr, &addr_len);
  if (_fd < 0)
    throw std::exception();
  _raw_request = std::string();
  _request = NULL;
  _response = NULL;
  _request_pipe[0] = _request_pipe[1] = -1;
  _response_pipe[0] = _response_pipe[1] = -1;
}

Client::~Client() {
  if (_request != NULL)
    delete _request;
  if (_response != NULL)
    delete _response;
}

/*
return 0 : not closed header
return num : new fd, must set to wfds
return -1 : 
*/

int  Client::recv(fd_set& all_wfds) {
  char    buf[BUFSIZE];
  int     n_read;
  size_t  header_end;

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
        std::cout << "header oversize" << std::endl;
        return (1);
      } else {
        if (header_end <= MAX_HEADER_SIZE) {
          try {
            /* make request */
            std::string req_header = _raw_request.substr(0, header_end + 4);
            std::string req_body = _raw_request.substr(header_end + 4);

            _request = new Request(req_header);
            pipe(_request_pipe);
            std::cout << "_request_pipe " << _request_pipe[0] << " " << _request_pipe[1] << std::endl;
            Fd::set(_request_pipe[1], all_wfds);
            _request->setBody(new Body(req_body.c_str(), req_body.size(), false));
            // if (_request->hasBody()) {
            //   _request->setBody(new Body(req_body.c_str(), req_body.size(), false));
            // }

            /* make response */
            _response = new Response(*this);

         } catch (HttpException & err) {
            std::cout << "exception:" << err.getStatus() << std::endl;
            Response response(err.getStatus());
            response.send(_fd);
            return (1);
          }

        } else {
          std::cout << "header oversize" << std::endl;
          return (1);         
        }
      }
    } else {
      _request->getBody()->recv(_fd);
    }
  } else {
    std::string msg = "error: n_read is " + std::to_string(n_read);
    std::cout << msg << std::endl;
    std::cout << strerror(errno) << std::endl;
    /* TODO: remove client */
    throw "n_read exception";

  }
  return (1);
}

int   Client::send() {
  int n_written = _request->getBody()->send(_request_pipe[1]);
  _n_sent += n_written;
  /* if all body data is sent, send EOF */
  std::cout << "_n_sent : " << _n_sent << " CL: " << _request->getContentLength() << std::endl;
  if (_n_sent == _request->getContentLength()) {
    std::cout << "_____________FULL____________" << std::endl;
    Fd::clearWfd(_request_pipe[1]);
    close(_request_pipe[1]);
  }
  return n_written;
}

int   Client::getFd() const {
  return _fd;
}

Request *Client::getRequest() {
  return _request;
}

Response *Client::getResponse() {
  return _response;
}

int *Client::getRequestPipe() {
  return _request_pipe;
}

int *Client::getResponsePipe() {
  return _response_pipe;
}

const std::vector<Location>&  Client::getLocations() const {
  return _server.getLocations();
}

const Server&                 Client::getServer() const {
  return _server;
}
