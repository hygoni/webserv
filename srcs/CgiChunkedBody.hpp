#ifndef SRCS_CGI_CHUNKED_BUFFER_HPP
#define SRCS_CGI_CHUNKED_BUFFER_HPP

/* Copyright 2020 hyeyoos */

#include <string>
#include <string.h>
#include "Body.hpp"

class CgiChunkedBody : public Body {
 private:
  int           _chunk_size;
  int           _n_sent;
  int           _pos;
  std::string   _chunked_read_buf;
  std::string   _chunked_write_buf;
  bool          isHex(char c);
 public:
                
                  CgiChunkedBody();
                  CgiChunkedBody(std::string const& s);
  virtual         ~CgiChunkedBody();
  virtual int     recv(int fd);
  virtual int     send(int fd);
  void            recvString(std::string s);
  virtual int     getChunkedContentLength() const;
  virtual bool    isChunkedReceived() const;
  virtual bool    isChunkedSent() const;

};

#endif  // SRCS_CGI_CHUNKED_BUFFER_HPP

