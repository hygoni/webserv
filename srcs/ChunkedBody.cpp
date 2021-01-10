#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include "ChunkedBody.hpp"
#include "libft.h"
#include "Fd.hpp"

/* Copyright 2020 hyeyoo */

ChunkedBody::ChunkedBody() : Body(0) {
  _chunk_size = -1;
  _pos = 0;
}

ChunkedBody::~ChunkedBody() {
  
}

bool  ChunkedBody::isHex(char c) {
  if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
    return true;
  return false;
}

void ChunkedBody::addBody(std::string const& s) {
  _buf.append(s);

  while (!isFinished()) {
    int i;
  
    if (_chunk_size == -1) {
      int chunk_size = 0;
      for (i = 0; i < _buf.length() && isHex(_buf.at(i)); i++) {
        chunk_size *= 16;
        if (ft_isdigit(_buf.at(i)))
          chunk_size += _buf.at(i) - '0';
        else
          chunk_size += ft_tolower(_buf.at(i)) - 'a' + 10;
      }
      if (_buf.length() == i || (_buf.at(i) == '\r' && _buf.length() == i + 1))
        return ;
      if (_buf.length() >= i + 2 && !(_buf.at(i) == '\r' && _buf.at(i + 1) == '\n'))
        throw "Bad request";
      _chunk_size = chunk_size;
      _pos = i;
    }
    if (_buf.length() >= _pos + 2 + _chunk_size + 2) {
      if (_buf.substr(_pos + 2 + _chunk_size, 2).compare("\r\n") != 0) {
        throw "Bad request: size and data mismatch";
      }
      _body.append(_buf.substr(_pos + 2, _chunk_size));
      _buf = _buf.substr(_pos + 2 + _chunk_size + 2);
      _pos = 0;
      /* if chunked is ended, don't reset chunk size */
      if (_chunk_size == 0) {
        return ;
      }
      _chunk_size = -1;
    } else {
      return ;
    }
  }
}

bool ChunkedBody::isFinished() const {
  return (_chunk_size == 0 && _pos == 0);
}

/* when finished, data in buf is remained data (next request) */
std::string ChunkedBody::getRemain() const {
  return _buf;
}