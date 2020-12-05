#include "Body.hpp"

#ifndef BUFSIZE
# define BUFSIZE 8192
#endif

Body::Body(bool is_transfer_encoding) : 
_buf(BUFSIZE), _is_transfer_encoding(is_transfer_encoding) {}

/* TODO: process transfer encoding */

int Body::recv(int fd) {
  return _buf->recv(fd);
}

int Body::send(int fd) {
  return _buf->send(fd);
}
