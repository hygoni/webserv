#include "Client.hpp"
#include "Response.hpp"
#include "Fd.hpp"
#include "CGI.hpp"
#include "CgiChunkedBody.hpp"
#include "ChunkedBody.hpp"
#include "CgiBody.hpp"
#include "debug.hpp"
#include "Base64.hpp"
#define MAX_HEADER_SIZE 8192
#include <arpa/inet.h>


Client::Client
(Server const& server) : _server(server) {
  socklen_t     addr_len;
  sockaddr_in   client_addr;

  addr_len = sizeof(client_addr);
  _fd = accept(server.getFd(), (struct sockaddr *)&client_addr, &addr_len);
  if (_fd < 0)
    throw "[Client::Client] bad file descriptor";
  gettimeofday(&_created, NULL);
  _raw_request = std::string();
  _request = NULL;
  _response = NULL;
  _request_pipe[0] = _request_pipe[1] = -1;
  _response_pipe[0] = _response_pipe[1] = -1;
  _n_sent = 0;
  _is_cgi_executed = false;
  _cgi_path = "";
}

Client::Client(Client const& client) : _server(client._server) {
  _fd = client._fd;
  _raw_request = client._raw_request;
  _request = client._request;
  _response = client._response;
  _request_pipe[0] = client._request_pipe[0];
  _request_pipe[1] = client._request_pipe[1];
  _response_pipe[0] = client._response_pipe[0];
  _response_pipe[1] = client._response_pipe[1];
  _n_sent = client._n_sent;
  _is_cgi_executed = client._is_cgi_executed;
  _cgi_path = client._cgi_path;
  _cgi_file_path = client._cgi_file_path;
  _location = client._location;
}

Client const& Client::operator=(Client const& client) {
  _fd = client._fd;
  _raw_request = client._raw_request;
  _request = client._request;
  _response = client._response;
  _request_pipe[0] = client._request_pipe[0];
  _request_pipe[1] = client._request_pipe[1];
  _response_pipe[0] = client._response_pipe[0];
  _response_pipe[1] = client._response_pipe[1];
  _n_sent = client._n_sent;
  _is_cgi_executed = client._is_cgi_executed;
  _cgi_path = client._cgi_path;
  _cgi_file_path = client._cgi_file_path;
  _location = client._location;
  return *this;
}

Client::~Client() {
  log("[Client::~Client] destructor called\n");
  if (_request != NULL)
    delete _request;
  if (_response != NULL)
    delete _response;
  
  Fd::clearRfd(_request_pipe[0]);
  Fd::clearWfd(_request_pipe[1]);
  Fd::clearRfd(_response_pipe[0]);
  Fd::clearWfd(_response_pipe[1]);
  Fd::clearRfd(_fd);
  Fd::clearWfd(_fd);
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

int  Client::recv(fd_set& all_wfds) {
  char    buf[BUFSIZE];
  int     n_read;
  size_t  header_end;

  if (_request == NULL) {
    /* Connection closed or Error occurred */
    if ((n_read = ::recv(_fd, buf, BUFSIZE - 1, 0)) <= 0) {
      return -1;
    }
    buf[n_read] = '\0';
    _raw_request += buf;

    header_end = _raw_request.find("\r\n\r\n");
    if (header_end == std::string::npos) {
      if (_raw_request.size() <= MAX_HEADER_SIZE)
        return (0);
      return (1);
    } else {
      if (header_end <= MAX_HEADER_SIZE) {
        try {
          /* make request */
          std::string req_header = _raw_request.substr(0, header_end + 4);
          std::string req_body = _raw_request.substr(header_end + 4);

          _request = new Request(req_header);
          setLocation();
          setCgiPath();
          /* authenticate */
          if (_location->getAuthorization().find(':') != std::string::npos) {
            if (!this->auth())
              return (1);
          }
          /* make response */
          _response = new Response(*this);
          if (_request->hasBody()) {
            pipe(_request_pipe);
            Fd::setWfd(_request_pipe[1]);  
            if (_request->isChunked()) {
              if (_response->isCgi()) {
                _request->setBody(new CgiChunkedBody(req_body));
              } else {
                _request->setBody(new ChunkedBody(req_body));
              }                                                                                    
            } else {
              _request->setBody(new Body(req_body));
            }           
          }
         } catch (HttpException & err) {
            std::cout << "exception:" << err.getStatus() << std::endl;
            if (_response != NULL)
              delete _response;
            _response = new Response(*this, err.getStatus());
            return (1);
          }
        } else {
          std::cout << "header oversize" << std::endl;
          return (1);         
        }
      } 
    } else {
      if (_request->hasBody())
        _request->getBody()->recv(_fd);
    }
  return (1);
}

int   Client::send() {
  size_t      idx;

  if (_request->isChunked() && _cgi_path != "") {
    if (_request->getBody()->isChunkedClosed()) {
      if (_is_cgi_executed) {
        int n_written = _request->getBody()->send(_request_pipe[1]);
      } else {
        size_t content_length = _request->getBody()->getChunkedContentLength();
        (*_request->getHeader())["Content-Length"] = std::to_string(content_length);
        Cgi cgi(*this);
        cgi.run();
        _is_cgi_executed = true;
      }
    }
  } else {
    int n_written = _request->getBody()->send(_request_pipe[1]);
    _n_sent += n_written;
    if (_n_sent == _request->getContentLength()) {
      std::cout << "_n_sent : " << _n_sent << " CL: " << _request->getContentLength() << std::endl;
      std::cout << "_____________FULL____________" << std::endl;
      Fd::clearWfd(_request_pipe[1]);
      close(_request_pipe[1]);
    }
  }  
  return 0;
}

void  Client::setLocation() {
  for (std::vector<Location>::const_iterator l_it = getLocations().begin();
      l_it != getLocations().end(); l_it = std::next(l_it)) {
    /* matching location found */
    log("[Client::setLocation] locaation: %s\n", l_it->getPath().c_str());
    if (_request->getTarget().find(l_it->getPath()) == 0) {
      _location = &(*l_it);
      return ;
    }
  }
  return throw HttpException(404);
}

void Client::setCgiPath() {
  size_t idx;
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

  char buf[BUFSIZE];
  if (getcwd(buf, BUFSIZE) < 0)
    throw "[Client::setCgiPath] getcwd failed";
  _cgi_file_path = std::string(buf) + "/" + (_location->getRoot() + _request->getTarget().substr(_location->getPath().length()));
}

bool  Client::auth() {
  bool is_authenticated;

  if (_request->auth(_location->getAuthorization()))
    return true;
  _response = new Response(*this, 401);
  (*_response->getHeader())["WWW-Authenticate"] = "Basic realm=\"Need Auth\"";
  return is_authenticated;
}

bool  Client::checkAlive() const {
  struct timeval now;

  gettimeofday(&now, NULL);
  if (now.tv_sec - _created.tv_sec < TIMEOUT_SEC)
    return true;
  return false;
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
