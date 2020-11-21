#ifndef SRCS_RESPONSE_HPP_
#define SRCS_RESPONSE_HPP_

#include <map>
#include "Server.hpp"
#include "Request.hpp"
#define BUFSIZE 4096

class Response {
 private:
  int          _offset;
  std::string  _response;
  /*
   * if _read_fd or _write_fd is negative,
   * it does not read or write
   */
  int          _read_fd; /* read from file */
  int          _write_fd; /* write to file */
  char         _buf[BUFSIZE + 1];
  int          _len;
  bool         _ready; /* it's ready to write  */
 public:
        Response(Request const& request, Server const& server);
        Response(int status);
   /* process request methods */
  bool  process(Request const& request, Server const& server);
  void  processByMethod(Request const& request, Location const& location);
  void  processGetMethod(Request const& request, Location const& location);
  void  processHeadMethod(Request const& request, Location const& location);
  void  processPostMethod(Request const& request, Location const& location);
  void  addStatusLine(int status);
  void  addHeader(std::string key, std::string value);
  void  endHeader();
  void  addBody(std::string const& content);
  int   send(int client_fd);
  int   read();
  int   getReadFd() const;
  int   getWriteFd() const;
  bool  isReadyToWrite() const;
};

#endif  // SRCS_RESPONSE_HPP_
