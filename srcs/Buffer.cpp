#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "Buffer.hpp"
#include "libft.h"
#include "Fd.hpp"

/* Copyright 2020 hyeyoo */

Buffer::Buffer(size_t size) {
  _len  = 0;
  _size = size;
  _buf = (char*)malloc(sizeof(char) * (size + 1));
}

Buffer::~Buffer() {
  free(_buf);
}

int Buffer::recv(int fd) {
  int n_read;

  /* when buffer isn't empty, don't receive */
  if (!isEmpty()) {
    return 0;
  }

  if ((n_read = read(fd, _buf, _size)) < 0)
    throw "[Buffer::send]: read failed";
  _buf[n_read] = '\0';
  _len = n_read;
  return n_read;
}

int Buffer::send(int fd) {
  int n_written;
  if ((n_written = write(fd, _buf, _len)) < 0)
    throw "[Buffer::send]: write failed";
  /* not all data of buffer was written */
  if (n_written < _len) {
    _len -= n_written;
    ft_memmove(_buf, _buf + n_written, _len);
    _buf[_len] = '\0';
  }
  return n_written;
}

bool Buffer::isEmpty() const {
  return _len == 0;
}

void Buffer::setBuffer(const char *buf, size_t size) {
  if (size > _size)
    throw "[Buffer::setBuffer]: size too long";
  _len = size;
  ft_memcpy(_buf, buf, _len);
  _buf[_len] = '\0';
  
}

