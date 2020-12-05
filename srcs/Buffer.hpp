#ifndef SRCS_BUFFER_HPP
#define SRCS_BUFFER_HPP

/* Copyright 2020 hyeyoo */

#include <string.h>

class Buffer {
 private:
  size_t  _len;
  size_t  _size;
  char    *_buf;

 public:
        Buffer(size_t size);
        ~Buffer();
  bool  isEmpty();
  int   recv(int fd);
  int   send(int fd);
  void  setBuffer(char *buf, size_t size);
};

#endif  // SRCS_BUFFER_HPP

