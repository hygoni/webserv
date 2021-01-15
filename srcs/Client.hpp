#ifndef SRCS_CLIENT_HPP_
#define SRCS_CLIENT_HPP_

#include "Request.hpp"
#include "Location.hpp"
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#define TIMEOUT_SEC 3000

class Response;
class Server;
class Client {
private:
  int             _id;
  int             _fd;
  int             _request_pipe[2];
  int             _response_pipe[2];
  int             _n_sent; // request body sent;
  bool            _is_cgi_executed;
  bool            _is_timeout;
  Request*        _request;
  Response*       _response;
  std::string     _raw_request;
  std::string     _cgi_path;
  std::string     _cgi_file_path;
  struct timeval  _created;
  Server const&   _server;
  const Location* _location;
  static char     *_buf;
  bool            _connection_closed;
  void        setLocation();
  void        setCgiPath();
              Client();
public:
  static int      num;
  int             id;
              Client(const Server& server);
              Client(Client const& client);
  Client const&   operator=(Client const& client);
              ~Client();
  void        clear();
  int         recv(fd_set const& fds);
  int         send(int fd);
  bool        auth();
  void        timeout();
  bool        isTimeout();
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
