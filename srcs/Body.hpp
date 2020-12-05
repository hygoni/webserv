#ifndef SRCS_BODY_HPP_
#define SRCS_BODY_HPP_

#include "Buffer.hpp"

class Body {
 private:
  bool    _is_transfer_encoding;
  Buffer  _buf;

 public:
        Body(bool is_transfer_encoding);
        Body(const char *buf, size_t size, bool is_transfer_encoding);
  int   recv(int fd);
  int   send(int fd);
};

#endif  // SRCS_BODY_HPP_

