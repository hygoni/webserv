#ifndef SRCS_RESPONSE_HPP_
#define SRCS_RESPONSE_HPP_

#include <map>
#include <unistd.h>
#include <dirent.h>
#include "Server.hpp"
#include "Request.hpp"
#include "Body.hpp"
#include "Header.hpp"
#include "debug.hpp"

class Response {
 private:
  static char* _buf;
  bool         _is_header_sent;
  bool         _is_cgi;
  Header       *_header;
  Body         *_body;
  int           _file_fd;
  pid_t        _cgi_pid;
  int          _pos_cgi;
  int          _pos;
  Client&      _client;

  Header*   initHeader(int status) const;

  /* process request methods */
  void  process(Client& client);
  void  processByMethod(Client& client, Location const& location, std::string const& path);
  void  processGetMethod(Client& client, Location const& location, std::string path);
  void  processHeadMethod(std::string const& path);
  void  processPutMethod(Client& client, Location const& location);
  void  processDeleteMethod(Client& client, Location const& location);
  void  processPostMethod();
  void  processCgi(Client& client);
  void  processDirectoryListing(Client& client, std::string const& path);
  void  processDefaultErrorPage(int status);

 public:
        Response(Client& client);
        Response(Client& client, int status);
        ~Response();
  int   recv(const fd_set *rfds, const fd_set *wfds);
  int   send(int fd);
  void  setStatus(int status);
  pid_t getCgiPid() const;
  void  setCgiPid(pid_t pid);
  bool  isCgi() const;
  Header* getHeader();
  int     getFileFd() const;
};

#endif  // SRCS_RESPONSE_HPP_
