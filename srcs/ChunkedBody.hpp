#ifndef SRCS_CHUNKED_BUFFER_HPP
#define SRCS_CHUNKED_BUFFER_HPP

/* Copyright 2020 hyeyoos */

#include <string>
#include <string.h>
#include "Body.hpp"

class ChunkedBody : public Body {
 protected:
  std::string   _buf;
  int           _pos;
  int           _chunk_size;
  bool          isHex(char c);
 public:
                              ChunkedBody();
  virtual                     ~ChunkedBody();
  virtual bool                isFinished() const;
  virtual std::string         getRemain() const;
  virtual void                addBody(std::string const& s);
};
#endif  // SRCS_CHUNKED_BUFFER_HPP

