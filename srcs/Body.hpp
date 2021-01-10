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
  int             _content_length;
  std::string     _body;

  Body();
 public:
                              Body(int content_length);
  virtual                     ~Body();
  virtual bool                isFinished() const;
  virtual std::string         getRemain() const;
  virtual void                addBody(std::string const& s);
  std::string const&          toString() const;
};

#endif  // SRCS_BODY_HPP_

