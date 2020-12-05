#include "Body.hpp"

#ifndef BUFSIZE
# define BUFSIZE 8192
#endif

Body::Body(bool is_transfer_encoding) : 
 _is_transfer_encoding(is_transfer_encoding), _buf(BUFSIZE) {}

Body::Body(const char* buf, size_t size, bool is_transfer_encoding) : 
 _is_transfer_encoding(is_transfer_encoding), _buf(BUFSIZE) {
   _buf.setBuffer(buf, size);
}

/* TODO: process transfer encoding */

int Body::recv(int fd) {
  return _buf.recv(fd);
}

int Body::send(int fd) {
  return _buf.send(fd);
}
