#include "Body.hpp"

#ifndef BUFSIZE
# define BUFSIZE 8192
#endif

Body::Body() {
  _len  = 0;
  _size = BUFSIZE;
  _buf = (char*)malloc(sizeof(char) * (BUFSIZE + 1));
}

Body::Body(std::string const& s) {
  _len = s.length();
  _buf = (char*)malloc(sizeof(char) * (BUFSIZE + 1));
  ft_strlcpy(_buf, s.c_str(), _size);
}

Body::~Body() {
  if (_buf != NULL)
    free(_buf);
}

/* TODO: process transfer encoding */

int Body::recv(int fd) {
  int n_read;

  /* when buffer isn't empty, don't receive */
  if (!isEmpty()) {
    return 0;
  }

  if ((n_read = read(fd, _buf, _size)) < 0)
    throw "[Body::send]: read failed";
  _buf[n_read] = '\0';
  _len = n_read;
  return n_read;
}

int Body::send(int fd) {
  int n_written;
  if ((n_written = write(fd, _buf, _len)) < 0)
    throw "[Body::send]: write failed";
  /* not all data of buffer was written */
  if (n_written < _len) {
    _len -= n_written;
    ft_memmove(_buf, _buf + n_written, _len);
    _buf[_len] = '\0';
  }
  return n_written;
}

size_t  Body::getChunkedContentLength() const {
  return 0;
}

bool    Body::isChunkedClosed() const {
  return false;
}

bool Body::isEmpty() const {
  return _len == 0;
}
