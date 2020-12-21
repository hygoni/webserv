#ifndef SRCS_CGI_BUFFER_HPP
#define SRCS_CGI_BUFFER_HPP

/* Copyright 2020 hyeyoo */

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
class CgiBuffer : public Buffer{
 private:
  Header        *_header;
  std::string   _raw_header;
  std::string   _raw_body;
  bool          _is_header_sent;
  bool          _is_header_closed;
  bool          _is_body_closed;
  size_t        _n_sent;

 public:
                CgiBuffer(size_t size);
                ~CgiBuffer();
  int           process(int fd, char *buf, int len);
  void          parse(int& status, std::map<std::string, std::string>& parse_map);

  int           recv(int fd);
  int           send(int fd);
  bool          isEmpty();
};

#endif  // SRCS_CGI_BUFFER_HPP

