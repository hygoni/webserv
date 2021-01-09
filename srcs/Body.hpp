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
  static char     *_read_buf;
  std::string     _buf;

 public:
                  Body();
                  Body(std::string const& s);
  virtual         ~Body();
  virtual int     recv(int fd);
  virtual int     send(int fd);
  virtual int     getChunkedContentLength() const;
  virtual bool    isChunkedReceived() const; /* all input is received */
  virtual bool    isChunkedSent() const; /* all write buffer is flushed */
};

#endif  // SRCS_BODY_HPP_

