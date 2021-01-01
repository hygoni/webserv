#ifndef SRCS_RESPONSE_HPP_
#define SRCS_RESPONSE_HPP_

#include <map>
#include <unistd.h>
#include <dirent.h>
#include "Server.hpp"
#include "Request.hpp"
#include "Body.hpp"
#include "Header.hpp"

#define BUFSIZE 4096

class Response {
 private:
  bool      _is_header_sent;
  bool      _is_Cgi;
  Header    *_header;
  Body      *_body;
  int       _file_fd;
  pid_t     _cgi_pid;
  size_t    _n_sent;
  Client&   _client;

 public:
        Response(Client& client);
        Response(Client& client, int status);
        ~Response();
  int   recv(int fd);
  int   send(int fd);

  /* process request methods */
  void  process(Client& client);
  void  processByMethod(Client& client, Location const& location, std::string const& path);
  void  processGetMethod(Client& client, Location const& location, std::string path);
  void  processHeadMethod(Client& client, Location const& location, std::string const& path);
  void  processPutMethod(Client& client, Location const& location);
  void  processDeleteMethod(Client& client, Location const& location);
  void  processPostMethod();
  void  processCgi(Client& client, Location const& location);
  void  processDirectoryListing(Client& client, Location const& location, std::string const& path);
  void setStatus(int status);
  pid_t getCgiPid() const;
  void  setCgiPid(pid_t pid);
  bool  isCgi() const;
  Header* getHeader();
  Body*   getBody() {
    return _body;
  }
};

#endif  // SRCS_RESPONSE_HPP_
