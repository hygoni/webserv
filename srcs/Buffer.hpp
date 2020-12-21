#ifndef SRCS_BUFFER_HPP
#define SRCS_BUFFER_HPP

/* Copyright 2020 hyeyoo */

#include <string.h>

class Buffer {
 protected:
  size_t  _len;
  size_t  _size;
  char    *_buf;

 public:
                Buffer(size_t size);
  virtual       ~Buffer();
  virtual int   recv(int fd);
  virtual int   send(int fd);
  bool          isEmpty();
  void          setBuffer(const char *buf, size_t size);
};

#endif  // SRCS_BUFFER_HPP

