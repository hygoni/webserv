#ifndef REQUEST_HPP_
#define REQUEST_HPP_
#include <string>
#include <iostream>
#include "Header.hpp"
#include "ChunkedBody.hpp"
#include "HttpException.hpp"
#include "Base64.hpp"
/* Copyright 2020 hyeyoo, hyekim */

class Request {
private:
  Header      *_header;
  Body        *_body;
  size_t      _content_length;
  bool        _chunked;
  bool        _is_closed;
  std::string _user_name;
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
  Request(std::string const& raw);
  bool        isClosed() const;
  bool        hasBody() const;
  std::string getMethod() const;
  std::string getTarget() const;
  std::string getVersion() const;
  std::string getUserName() const;
  size_t      getContentLength() const;
  bool        isChunked() const;
  void        addBody(std::string const &s);
  bool        isBodyFinished() const;
  std::string getBodyRemain() const;
  Header*     getHeader();
  Body*       getBody();
  void        setBody();
  bool        auth(std::string const& user_str);
};

#endif
