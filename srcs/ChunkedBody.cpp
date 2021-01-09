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

ChunkedBody::ChunkedBody() : Body() {
  _chunk_size = -1;
}

ChunkedBody::ChunkedBody(std::string const& s) {
  _chunk_size = -1;
  recvString(s.c_str());
}

ChunkedBody::~ChunkedBody() {
  
}

bool  ChunkedBody::isHex(char c) {
  if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
    return true;
  return false;
}

bool ChunkedBody::isChunkedReceived() const {
  return (_chunk_size == 0);
}

bool ChunkedBody::isChunkedSent() const {
  return (_chunk_size == 0 && _chunked_write_buf.length() == 0);
}

void  ChunkedBody::recvString(std::string buf) {
  int  chunk_size = 0;
  size_t  i;
  
  _chunked_read_buf.append(buf);
  //log("_chunked_read_buf += %s\n", buf);
  if (_chunked_read_buf.length() == 0)
    return;
  else if (_chunked_read_buf.find("\r\n") == std::string::npos)
    return;
  /* if there's no size, parse size */
  if (_chunk_size == -1) {
    for (i = 0; i < _chunked_read_buf.length() && isHex(_chunked_read_buf.at(i)); i++) {
      chunk_size *= 16;
      if (ft_isdigit(_chunked_read_buf.at(i)))
        chunk_size += _chunked_read_buf.at(i) - '0';
      else
        chunk_size += ft_tolower(_chunked_read_buf.at(i)) - 'a' + 10;
    }
    /* if no \r\n after size */
    if (i == 0 || (_chunked_read_buf.at(i) != '\r' && _chunked_read_buf.at(i + 1) != '\n')) {
      log("===== invalid chunked body1 =====\n");
      log("chunked_read_buf = %s\n", _chunked_read_buf.c_str());
      log("_chunk_size = %d\n", chunk_size);
      throw "[ChunkedBody::recv] Invalid Chunked Body1";
    }
    _chunked_read_buf = _chunked_read_buf.substr(i + 2);
    _chunk_size = chunk_size;
    /* parsed size, but not read chunk-data yet */
  }
  log("[ChunkedBody::recvString] _chunk_size = %d, chunked_read_buf.length(%d)\n", _chunk_size, _chunked_read_buf.length());

  if (_chunk_size != -1 && _chunked_write_buf.length() == 0) {
    if ((int)_chunked_read_buf.length() >= _chunk_size + 2) {
      _chunked_write_buf = _chunked_read_buf.substr(0, _chunk_size);
      if (_chunked_read_buf.substr(_chunk_size, 2).compare("\r\n") != 0)
        throw "[ChunkedBody::recv] Invalid Chunked Body2";
      _chunked_read_buf = _chunked_read_buf.substr(_chunk_size + 2);
    }
    log("[ChunkedBody::recvString] make write_buf.length = %u\n", _chunked_write_buf.length());
  }
}

int ChunkedBody::recv(int fd) {
  int     n_read;

  if ((n_read = read(fd, _read_buf, BUFSIZE)) < 0) {
    log("[ChunkedBody::recv]: read failed");
    return -1;
  }
  _read_buf[n_read] = '\0';
  log("[ChunkedBody:recv] received(%d), current chunk_size(%d)\n", n_read, _chunk_size);
  recvString(std::string(_read_buf, n_read));

  return n_read;
}

int ChunkedBody::send(int fd) {
  int n_written;

  recvString(""); /* data in buffer should be processed */
  int len = std::min(BUFSIZE, (int)_chunked_write_buf.length());
  if ((n_written = write(fd, _chunked_write_buf.c_str(), len)) < 0) {
    log("[ChunkedBody::send]: write failed");
    return -1;
  }
  _chunked_write_buf.erase(0, n_written);
  if (n_written > 0 && _chunked_write_buf.length() == 0 && _chunk_size != 0) {
    _chunk_size = -1;
  }
  return n_written;
}
