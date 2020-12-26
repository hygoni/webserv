#ifndef SRCS_CHUNKED_BUFFER_HPP
#define SRCS_CHUNKED_BUFFER_HPP

/* Copyright 2020 hyeyoos */

#include <string>
#include <string.h>
#include "Buffer.hpp"

class ChunkedBuffer : public Buffer {
 private:
  int           _chunk_size;
  int           _n_sent;
  std::string   _chunked_read_buf;
  std::string   _chunked_write_buf;
  bool          isHex(char c);
 public:
                ChunkedBuffer(size_t size);
  virtual       ~ChunkedBuffer();
  virtual int   recv(int fd);
  virtual int   send(int fd);
  virtual bool  isEmpty() const;
  bool          isClosed() const;
};

#endif  // SRCS_CHUNKED_BUFFER_HPP

