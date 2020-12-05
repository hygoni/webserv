#ifndef SRCS_RESPONSE_HPP_
#define SRCS_RESPONSE_HPP_

#include <map>
#include <unistd.h>
#include "Server.hpp"
#include "Request.hpp"
#include "Body.hpp"
#include "Header.hpp"

#define BUFSIZE 4096

class Response {
 private:
  bool      _is_header_sent;
  Header    _header;
  Body      _body;
  pid_t     _cgi_pid;

 public:
        Response(Request const& request, std::vector<Location> const& locations);
        Response(int status);
  int   recv(int fd);
  int   send(int fd);

  /* process request methods */
  bool  process(Request const& request, std::vector<Location> const& locations);
  void  processByMethod(Request const& request, Location const& location);
  void  processGetMethod(Request const& request, Location const& location);
  void  processHeadMethod(Request const& request, Location const& location);
  void  processPostMethod(Request const& request, Location const& location);
  void  processCgi(Request const& request, Location const& location);


  pid_t getCgiPid() const;
  void  setCgiPid(pid_t pid);
};

#endif  // SRCS_RESPONSE_HPP_
