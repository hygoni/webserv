#ifndef SRCS_CGI_BUFFER_HPP
#define SRCS_CGI_BUFFER_HPP

/* Copyright 2020 hyeyoo */

#include "Header.hpp"
#include "Body.hpp"
#include <string.h>
#include <string>
#include <map>

/* Copyright 2020 hyeyoo */

/*
 * CgiMiddleware
 * if response is from CGI
 * parse and processes response
 * according to RFC 3875 (Section 6 - CGI Response)
 */
class CgiBody : public Body {
 protected:
  Header        **_header;
  std::string   _raw_header;
  std::string   _raw_body;
  bool          _is_header_sent;
  bool          _is_header_closed;
  bool          _is_body_closed;
  int           _pos;
  int           process(int fd, char *buf, int len);
  void          parse(int& status, std::map<std::string, std::string>& parse_map);
  void          saveMap(Header& header, std::map<std::string, std::string>& parse_map);

 public:
                              CgiBody(Header **header);
                              CgiBody(const std::string& s, Header **header);
                              ~CgiBody();
  virtual bool                isFinished() const;
  virtual std::string         getRemain() const;
  virtual void                addBody(std::string const& s);
};

#endif  // SRCS_CGI_BUFFER_HPP

