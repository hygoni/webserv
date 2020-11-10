#ifndef REQUEST_HPP_
#define REQUEST_HPP_
#include <string>
#include <iostream>
#include <map>
#include "HttpException.hpp"
/* Copyright 2020 hyeyoo, hyekim */

class Request {
private:
  std::string _method;
  std::string _target;
  std::string _version;
  std::map<std::string, std::string>  _headers;

  Request();
  std::pair<std::string, std::string> makeHeader(std::string header_field);
  void  initMethod(std::string raw);
  void  initTarget(std::string raw);
  void  initVersion(std::string raw);
  void  initStartLine(std::string raw);
  void  initHeaders(std::string raw);
public:
  Request(std::string raw);
  void  debugOstream(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const Request& request);
#endif
