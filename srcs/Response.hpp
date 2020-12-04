#ifndef SRCS_RESPONSE_HPP_
#define SRCS_RESPONSE_HPP_

#include <map>
#include <unistd.h>
#include "Server.hpp"
#include "Request.hpp"
#define BUFSIZE 4096

class Response {
 private:
  int          _offset;
  std::string  _response;
  /*
   * if fd is negative,
   * it's not using it
   */
  int          _body_write_fd; /* write end of body pipe */
  char         _body_buf[BUFSIZE + 1];
  size_t       _body_length;
  int          _response_read_fd; /* read end of response */
  char         _response_buf[BUFSIZE + 1];
  size_t       _response_length;
  pid_t        _cgi_pid;
 public:
        Response(Request const& request, std::vector<Location> const& locations);
        Response(int status);
   /* process request methods */
  bool  process(Request const& request, std::vector<Location> const& locations);
  void  processByMethod(Request const& request, Location const& location);
  void  processGetMethod(Request const& request, Location const& location);
  void  processHeadMethod(Request const& request, Location const& location);
  void  processPostMethod(Request const& request, Location const& location);
  void  processCgi(Request const& request, Location const& location);
  int   send(int fd);
  void  addStatusLine(int status);
  void  addHeader(std::string key, std::string value);
  void  endHeader();
  void  addBody(std::string const& content);
 
  int   readBody();
  int   writeBody();
  int   closeBody();
  int   readResponse();
  int   writeResponse();


  void  putBodyBuffer(char *buf, size_t size);

  /* getters */
  char* getBodyBuffer();
  char* getResponseBuffer();
  int   getBodyWriteFd() const;
  int   getResponseReadFd() const;
  size_t   getBodyLength() const;
  size_t   getResponseLength() const;
  pid_t getCgiPid() const;
  bool  isBodyReady() const;
  bool  isResponseReady() const;
  /* setters */
  void  setBodyLength(size_t len);
  void  setResponseReadFd(int fd);
  void  setBodyWriteFd(int fd);
  void  setCgiPid(pid_t pid);
};

#endif  // SRCS_RESPONSE_HPP_
