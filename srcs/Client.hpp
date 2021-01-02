#ifndef SRCS_CLIENT_HPP_
#define SRCS_CLIENT_HPP_

#include "Request.hpp"
#include "Location.hpp"
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#define TIMEOUT_SEC 30

class Response;
class Server;
class Client {
private:
  int             _fd;
  int             _request_pipe[2];
  int             _response_pipe[2];
  int             _n_sent; // request body sent;
  bool            _is_cgi_executed;
  Request*        _request;
  Response*       _response;
  std::string    _raw_request;
  std::string     _cgi_path;
  std::string     _cgi_file_path;
  struct timeval  _created;
  Server const&                 _server;
  const Location*               _location;

  void        setLocation();
  void        setCgiPath();
              Client();
public:
              Client(const Server& server);
              Client(Client const& client);
  Client const&   operator=(Client const& client);
              ~Client();
  int         recv(fd_set& all_wfds);
  int         send();
  bool        auth();
  bool        checkAlive() const;
  bool        isCgi() const;

  Request*    getRequest();
  Response*   getResponse();
  int*        getRequestPipe();
  int*        getResponsePipe();
  int         getFd() const;
  const std::string&            getCgiPath() const;
  const std::string&            getCgiFilePath() const;
  const std::vector<Location>&  getLocations() const;
  const Location*               getLocation() const;
  const Server&                 getServer() const;
};

#endif  //  SRCS_CLIENT_HPP_
