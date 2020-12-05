#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include "Buffer.hpp"
#include "libft.h"

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
  if (!isEmpty())
    return 0;

  if ((n_read = ::recv(fd, _buf, _size, 0)) < 0)
    throw std::exception();
  _buf[n_read] = '\0';
  _len = n_read;
  return n_read;
}

int Buffer::send(int fd) {
  int n_written;
  
  if ((n_written = ::send(fd, _buf, _len, 0)) < 0)
    throw std::exception();

  /* not all data of buffer was written */
  if (n_written < _len) {
    _len -= n_written;
    ft_memmove(_buf, _buf + n_written, _len);
    _buf[_len] = '\0';
  }

  return n_written;
}

bool Buffer::isEmpty() {
  return _len == 0;
}

void Buffer::setBuffer(char *buf, size_t size) {
  if (size > _size)
    throw std::exception();
  ft_memcpy(_buf, buf, size);
  _buf[size] = '\0';
}

