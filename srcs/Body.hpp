#ifndef SRCS_BODY_HPP_
#define SRCS_BODY_HPP_

#include <string>
#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "libft.h"

class Body {
 protected:
  size_t  _len;
  size_t  _size;
  char    *_buf;

 public:
                  Body();
                  Body(std::string const& s);
  virtual         ~Body();
  virtual int     recv(int fd);
  virtual int     send(int fd);
  virtual size_t  getChunkedContentLength() const;
  virtual bool    isChunkedClosed() const;
  bool            isEmpty() const;

};

#endif  // SRCS_BODY_HPP_

