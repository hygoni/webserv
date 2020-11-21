#ifndef SRCS_RESPONSE_HPP_
#define SRCS_RESPONSE_HPP_

#include <map>
#include "Server.hpp"
#include "Request.hpp"

class Response {
 private:
  int          _offset;
  std::string  _response;
 public:
        Response(Request const& request);
        Response(int status);
   /* process request methods */
  bool  process(Request const& request, Server const& server);
  void  processByMethod(Request const& request, Location const& location);
  void  processGETMethod(Request const& request, Location const& location);
  void  processHEADMethod(Request const& request, Location const& location);
  void  processPOSTMethod(Request const& request, Location const& location);
  void  addStatusLine(int status);
  void  addHeader(std::string key, std::string value);
  void  endHeader();
  void  addBody(std::string const& content);
  int   send(int fd);
};

#endif  // SRCS_RESPONSE_HPP_
