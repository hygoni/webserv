#ifndef REQUEST_HPP_
#define REQUEST_HPP_
#include "Header.hpp"
#include "HttpException.hpp"
#include <string>
#include <iostream>
#define MAX_BODY_SIZE 40000
/* Copyright 2020 hyeyoo, hyekim */

class Request {
private:
  std::string _body;

  Header      _header;
  size_t      _content_length;
  bool        _chunked;
  bool        _is_closed;

  Request();
  void  initStartLine(std::string raw);
  void  initHeaders(std::string raw);
  void  addHeader(std::string header_field);

  std::string  parseChunk(const std::string & chunk);
  void  checkOverlapHeader(const std::string & key, const std::string & value) const;
  void  checkHeaders();
  void  checkClosed();
public:
  ~Request();
  Request(std::string raw);
  void  debugOstream(std::ostream& os) const;
  std::string addBody(const std::string & str);
  bool  isClosed() const;

  std::string getMethod() const;
  std::string getTarget() const;
  std::string getVersion() const;
  std::string getBody() const;
  Header getHeader() const;
};

std::ostream& operator<<(std::ostream& os, const Request& request);
#endif
