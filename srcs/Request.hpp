#ifndef REQUEST_HPP_
#define REQUEST_HPP_
#include <string>
#include <iostream>
#include "Header.hpp"
#include "Body.hpp"
#include "HttpException.hpp"
#include "Base64.hpp"
#define MAX_BODY_SIZE 40000
/* Copyright 2020 hyeyoo, hyekim */

class Request {
private:
  Header      *_header;
  Body        *_body;
  size_t      _content_length;
  bool        _chunked;
  bool        _is_chunked_closed;
  bool        _is_closed;
  std::string _cgi_path;

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
  bool  hasBody() const;
  std::string getMethod() const;
  std::string getTarget() const;
  std::string getVersion() const;
  size_t         getContentLength() const;
  bool           isChunked() const;
  bool           isChunkedClosed() const;
  void           setChunkedClosed();
  Header *getHeader();
  Body   *getBody();
  void   setBody(Body *body);
  bool   auth(std::string const& user_str) const;
};

std::ostream& operator<<(std::ostream& os, const Request& request);
#endif
