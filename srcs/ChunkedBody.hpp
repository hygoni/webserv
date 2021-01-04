#ifndef SRCS_CHUNKED_BUFFER_HPP
#define SRCS_CHUNKED_BUFFER_HPP

/* Copyright 2020 hyeyoos */

#include <string>
#include <string.h>
#include "Body.hpp"

class ChunkedBody : public Body {
 private:
  int           _chunk_size;
  int           _n_sent;
  std::string   _chunked_read_buf;
  std::string   _chunked_write_buf;
  bool          isHex(char c);
 public:
                ChunkedBody();
                ChunkedBody(std::string const& s);
  void          recvString(std::string);
  virtual       ~ChunkedBody();
  virtual int   recv(int fd);
  virtual int   send(int fd);
  virtual bool  isChunkedClosed() const;
};

#endif  // SRCS_CHUNKED_BUFFER_HPP

