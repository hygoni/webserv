#include "Client.hpp"
#include "Response.hpp"
#include "Fd.hpp"
#include "CGI.hpp"
#include "ChunkedBody.hpp"
#include "CgiBody.hpp"
#include "debug.hpp"
#include "Base64.hpp"
#include <stdlib.h>
#include <arpa/inet.h>
#define MAX_HEADER_SIZE 8192
int Client::num = 0;
char *Client::_buf = (char*)malloc(sizeof(char) * (BUFSIZE + 1));

Client::Client
(Server const& server) : _server(server) {
  socklen_t     addr_len;
  sockaddr_in   client_addr;

  id = (++Client::num);
  addr_len = sizeof(client_addr);
  _fd = accept(server.getFd(), (struct sockaddr *)&client_addr, &addr_len);
  if (_fd < 0)
    throw "[Client::Client] bad file descriptor";
  updateTime();
  _raw_request = "";
  _request = NULL;
  _response = NULL;
  _request_pipe[0] = _request_pipe[1] = -1;
  _response_pipe[0] = _response_pipe[1] = -1;
  _n_sent = 0;
  _is_cgi_executed = false;
  _is_timeout = false;
  _cgi_path = "";
  _cgi_file_path = "";
  _connection_closed = false;
}

void Client::updateTime() {
  gettimeofday(&_created, NULL);
}

void Client::clear() {
  debug_printf("[Client::clear] \n");
  if (_request != NULL) {
    delete _request;
    _request = NULL;
  }
  if (_response != NULL) {
    delete _response;
    _response = NULL;
  }
  Fd::close(_request_pipe[0]);
  Fd::close(_request_pipe[1]);
  Fd::close(_response_pipe[0]);
  Fd::close(_response_pipe[1]);
  Fd::clearWfd(_fd);
  _cgi_path.clear();
  _cgi_file_path.clear();
  _is_cgi_executed = false;
  _connection_closed = false;
  _n_sent = 0;
  _location = NULL;
  _is_timeout = false;
  _connection_closed = false;
  updateTime();
}

Client::~Client() {
  debug_printf("[Client::~Client] destructor called\n");
  this->clear();
  Fd::clearRfd(_fd);
  Fd::close(_fd);
}

// long     Client::ft_ntohl(long num) {
//   long result = 1;

//   if (((char *)&result)[0] == 0) {
//     return num;
//   }
//   for (size_t i = 0; i < sizeof(long); i++) {
//     ((char *)&result)[i] = ((char *)&num)[sizeof(long) - i];
//   }
//   return result;
// }

/*
return 0 : not closed header
return num : new fd, must set to wfds
return -1 :
*/

bool Client::isConnectionClosed() const {
  debug_printf("[Client::isConnectionClosed] IsSet Rfd : %d, Wfd : %d\n", Fd::isSet(_fd, Fd::rfds), Fd::isSet(_fd, Fd::wfds));
  debug_printf("[Client::isConnectionClosed] closed : %d, raw_request.length = %lu, _request = %p\n", _connection_closed, _raw_request.length(), _request);
  return (_connection_closed && _raw_request.length() == 0 && _request == NULL);
}

int  Client::recv_sub(const fd_set *fds) {
  int     n_read = 0;
  size_t  header_end;

  if (Fd::isSet(_fd, fds)) {
    n_read = read(_fd, _buf, BUFSIZE - 1);
    debug_printf("[Client::recv_sub] client %d, n_read = %d\n", _fd, n_read);
    if (n_read < 0)
      return -1;
    else if (n_read == 0)
      _connection_closed = true;
    updateTime();
  } else {
    debug_printf("[Client::recv_sub] client %d, isSet false\n", _fd);
  }

  if (_request == NULL) {
    _raw_request.append(std::string(_buf, n_read));
    header_end = _raw_request.find("\r\n\r\n");
    try {
      if (header_end == std::string::npos) {
        if (_raw_request.size() <= MAX_HEADER_SIZE)
          return 1;
        debug_printf("_raw_request.size(%lu) > MAX_HEADER_SIZE\n", _raw_request.length());
        throw HttpException(413);
      } else {
        if (header_end <= MAX_HEADER_SIZE) {
          /* make request */
          std::string raw_header = _raw_request.substr(0, header_end + 4);
          std::string raw_body = _raw_request.substr(header_end + 4);
          _raw_request.clear();

          _request = new Request(raw_header);
          setLocation();
          setCgiPath();
          if ((int)_request->getContentLength() > _location->getClientBodySizeLimit()) {
            debug_printf("[Client::recv] content length is over\n");
            throw HttpException(413);
          }

          /* authenticate */
          if (_location->getAuthorization().find(':') != std::string::npos && !this->auth()) {
            Fd::setWfd(_fd);
            return 1;
          }

          /* make request body */
          _request->setBody();
          _request->addBody(raw_body);
          if ((int)_request->getBody()->toString().length() > this->getLocation()->getClientBodySizeLimit()) {
            debug_printf("[Client::recv] bigger than client_body_size_limit\n");
            throw HttpException(413);
          }
          if (_request->isBodyFinished()) {
            _response = new Response(*this);
            /* body remained is next header */
            _raw_request = _request->getBodyRemain();
            debug_printf("[Client:recv] body remain len: %lu\n", _request->getBodyRemain().length());
            Fd::setWfd(_fd);
            return 0;
          }
        } else {
          /* header too long */
          debug_printf("[Client::recv] header too long!\n");
          throw HttpException(413);
        }
        return 1;
      }
    } catch (HttpException & err) {
      debug_printf("[Client::recv] HttpException : %d\n", err.getStatus());
      if (_response != NULL)
        delete _response;
      _response = new Response(*this, err.getStatus());
      _raw_request.clear();
      Fd::setWfd(_fd);
      return 1;
    } catch (const char* s) {
      /* Internal Server Error */
      debug_printf("[Client::recv] Internal Error : %s\n", s);
      if (_response != NULL)
        delete _response;
      _response = new Response(*this, 500);
      _raw_request.clear();
      Fd::setWfd(_fd);
      return 1;
    }
  } else if (_response == NULL) {
    /* Caution: raw body can have next header... how to do it? */
    _request->addBody(std::string(_buf, n_read));
    /* make response when recv is end */
    if (_request->isBodyFinished()) {

    if ((int)_request->getBody()->toString().length() > this->getLocation()->getClientBodySizeLimit()) {
      if (_response != NULL)
        delete _response;
      _response = new Response(*this, 413);
      _raw_request.clear();
      Fd::setWfd(_fd);
      return 1;
    }
      _response = new Response(*this);
      /* body remained is next header */
      _raw_request = _request->getBodyRemain();
      debug_printf("[Client::recv] body remain size : %lu\n", _request->getBodyRemain().length());
      Fd::setWfd(_fd);
      return 0;
    }
    return 1;
  } else {
    _raw_request.append(std::string(_buf, n_read));
  }
  return 1;
}

int  Client::recv(const fd_set *fds) {
  try {
    return recv_sub(fds);
  } catch (std::exception const& e) {
    debug_printf("[Client::recv] HttpException : 500\n");
    if (_response != NULL)
      delete _response;
    _response = new Response(*this, 500);
    _raw_request.clear();
    Fd::setWfd(_fd);
    return 1;
  }
}

void  Client::setLocation() {
  for (std::vector<Location>::const_iterator l_it = getLocations().begin();
      l_it != getLocations().end(); l_it = std::next(l_it)) {
    /* matching location found */
    debug_printf("[Client::setLocation] locaation: %s\n", l_it->getPath().c_str());
    if (_request->getTarget().find(l_it->getPath()) == 0) {
      _location = &(*l_it);
      return ;
    }
  }
  return throw HttpException(404);
}

void Client::setCgiPath() {
  size_t idx;

  if (_request->getMethod() != "POST")
    return ;
  if ((idx = _request->getTarget().rfind('.')) != std::string::npos) {
    const std::string target_extension = _request->getTarget().substr(idx);
    /* set cgi path */
    for (std::vector<std::string>::const_iterator s_it = _location->getCgiExtension().begin();
          s_it != _location->getCgiExtension().end(); s_it = std::next(s_it)) {
      if (*s_it == target_extension) {
        _cgi_path = _location->getCgiPath();
      }
    }
  }

  char buf[4096];
  if (getcwd(buf, 4096) == NULL)
    throw "[Client::setCgiPath] getcwd failed";
  _cgi_file_path = std::string(buf) + "/" + (_location->getRoot() + _request->getTarget().substr(_location->getPath().length()));
}

bool  Client::auth() {
  if (_request->auth(_location->getAuthorization()))
    return true;
  _response = new Response(*this, 401);
  (*_response->getHeader())["WWW-Authenticate"] = "Basic realm=\"Need Auth\"";
  return false;
}

bool  Client::isTimeout() {
  struct timeval now;

  if (_is_timeout == true)
    return true;
  gettimeofday(&now, NULL);
  if (now.tv_sec - _created.tv_sec < TIMEOUT_SEC)
    return false;
  _is_timeout = true;
  return true;
}

void  Client::timeout() {
  if (_response != NULL) {
    if (_response->getHeader()->getStatus() == 408)
      return;
    delete _response;
  }
  _response = new Response(*this, 408);
  Fd::setWfd(_fd);
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

bool Client::isCgi() const {
  return !_cgi_path.empty();
}

const std::string&            Client::getCgiPath() const {
  return _cgi_path;
}

const std::string&            Client::getCgiFilePath() const {
  return _cgi_file_path;
}

const std::vector<Location>&  Client::getLocations() const {
  return _server.getLocations();
}

const Location*               Client::getLocation() const {
  return _location;
}

const Server&                 Client::getServer() const {
  return _server;
}

void                          Client::setResponse(Response* response) {
  if (_response != NULL)
    delete _response;
  _response = response;
}
