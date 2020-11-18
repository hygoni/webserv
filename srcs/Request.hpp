#ifndef REQUEST_HPP_
#define REQUEST_HPP_
#include <string>
#include <iostream>
#include <map>
#include "HttpException.hpp"
#define MAX_BODY_SIZE 40000
/* Copyright 2020 hyeyoo, hyekim */

class Request {
private:
  std::string _method;
  std::string _target;
  std::string _version;
  std::string _body;
  std::map<std::string, std::string>  _headers;
  bool        _is_closed;

  Request();
  std::pair<std::string, std::string> makeHeader(std::string header_field);
  void  initMethod(std::string raw);
  void  initTarget(std::string raw);
  void  initVersion(std::string raw);
  void  initStartLine(std::string raw);
  void  initHeaders(std::string raw);

  std::string  parseChunk(const std::string & chunk);
public:
  ~Request();
  Request(std::string raw);
  void  debugOstream(std::ostream& os) const;
  void  addBody(const std::string & str);
  bool  isClosed() const;
};

std::ostream& operator<<(std::ostream& os, const Request& request);
#endif
