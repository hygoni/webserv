#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include "CgiChunkedBody.hpp"
#include "libft.h"
#include "Fd.hpp"
#include "debug.hpp"

/* Copyright 2020 hyeyoo */

CgiChunkedBody::CgiChunkedBody() {
  _chunk_size = -1;
  _pos = 0;
}

CgiChunkedBody::CgiChunkedBody(std::string const& s) {
  _chunk_size = -1;
  _pos = 0;
  recvString(s);
}

CgiChunkedBody::~CgiChunkedBody() {
  
}

/*
 * save size,
 * when size bytes are saved, flsuh by send()
 */

bool  CgiChunkedBody::isHex(char c) {
  if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
    return true;
  return false;
}

bool CgiChunkedBody::isChunkedReceived() const {
  return (_chunk_size == 0);
}

bool CgiChunkedBody::isChunkedSent() const {
  return (isChunkedReceived() && _chunked_write_buf.length() == _pos);
}

int CgiChunkedBody::recv(int fd) {
  int     n_read;
  
  if ((n_read = read(fd, _read_buf, BUFSIZE)) < 0)
    return -1;
  _read_buf[n_read] = '\0';
  recvString(std::string(_read_buf, n_read));
  log("[CgiChunkedBody::recv] current size : %d\n", (int)_chunked_write_buf.length());
  return n_read;
}

void CgiChunkedBody::recvString(std::string s) {
  size_t i;

  _chunked_read_buf.append(s);
  while (42) {
    int  chunk_size = 0;
    /* if no crlf, there isn't size. so wait for next recv */
    if (_chunked_read_buf.find("\r\n") == std::string::npos)
      break;
    /* if there's no size, parse size */
    if (_chunk_size == -1) {
      log("[CgiChunkedBody::recv] size is -1, parsing... ");
      for (i = 0; i < _chunked_read_buf.length() && isHex(_chunked_read_buf.at(i)); i++) {
        chunk_size *= 16;
        if (ft_isdigit(_chunked_read_buf.at(i)))
          chunk_size += _chunked_read_buf.at(i) - '0';
        else
          chunk_size += ft_tolower(_chunked_read_buf.at(i)) - 'a' + 10;
      }
      _chunk_size = chunk_size;  
      if (_chunk_size > 0) {
        /* if no \r\n after size */
        if (i == 0 || (_chunked_read_buf.at(i) != '\r' && _chunked_read_buf.at(i + 1) != '\n'))
          throw "[CgiChunkedBody::recv] Invalid Chunked Body";
        _chunked_read_buf = _chunked_read_buf.substr(i + 2);
      }
      log("size = %d\n", _chunk_size);
    }
    /* parsed size, not added body yet */
    if (_chunk_size > 0 && (int)_chunked_read_buf.length() >= _chunk_size + 2) {
      // log("[CgiChunkedBody::recv] appending write buffer, content = %s\n", _chunked_read_buf.substr(0, _chunk_size).c_str());
      _chunked_write_buf.append(_chunked_read_buf.substr(0, _chunk_size));
      if (_chunked_read_buf.substr(_chunk_size, 2).compare("\r\n") != 0)
        throw "[CgiChunkedBody::recv] Invalid Chunked Body";
      _chunked_read_buf = _chunked_read_buf.substr(_chunk_size + 2);
      _chunk_size = -1;
    } else {
      break;
    }
  }
}

int CgiChunkedBody::send(int fd) {
  int n_written;

  /* all data is received */
  if (_chunk_size == 0) {
    int len = std::min(BUFSIZE, (int)_chunked_write_buf.length() - _pos);
    if ((n_written = write(fd, _chunked_write_buf.c_str() + _pos, len)) < 0)
      return -1;
    _pos += n_written;
    log("[CgiChunkedBody::send] _pos / _chunked_write_buf.length() = %d / %d\n", _pos, _chunked_write_buf.length());
    if (_pos == (int)_chunked_write_buf.length()) {
      log("[CgiChunkedBody::send] all chunked body is sent, closing.\n");
      /* close _request_pipe[1] (request->cgi) */
      close(fd);
      Fd::clearWfd(fd);
      return 0;
    }
    return n_written;
  }
  return 0;
}

int  CgiChunkedBody::getChunkedContentLength() const {
  return _chunked_write_buf.length();
}