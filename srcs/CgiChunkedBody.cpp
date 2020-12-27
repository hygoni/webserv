#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "CgiChunkedBody.hpp"
#include "libft.h"
#include "Fd.hpp"
#include "debug.hpp"

/* Copyright 2020 hyeyoo */

CgiChunkedBody::CgiChunkedBody() {
  _chunk_size = -1;
}

CgiChunkedBody::CgiChunkedBody(std::string const& s) {
  _chunk_size = -1;
  _chunked_read_buf += s;
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

bool CgiChunkedBody::isChunkedClosed() const {
  return (_chunk_size == 0);
}

int CgiChunkedBody::recv(int fd) {
  int     n_read;
  size_t  i;

  if ((n_read = read(fd, _buf, _size)) < 0)
    throw "[CgiChunkedBody::send]: read failed";
  _buf[n_read] = '\0';
  _chunked_read_buf += _buf;
  
  while (42) {
    size_t  chunk_size = 0;
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
    if (_chunk_size > 0 && _chunked_read_buf.length() >= _chunk_size + 2) {
      log("[CgiChunkedBody::recv] appending write buffer, content = %s\n", _chunked_read_buf.substr(0, _chunk_size).c_str());
      _chunked_write_buf.append(_chunked_read_buf.substr(0, _chunk_size));
      if (_chunked_read_buf.substr(_chunk_size, 2).compare("\r\n") != 0)
        throw "[CgiChunkedBody::recv] Invalid Chunked Body";
      _chunked_read_buf = _chunked_read_buf.substr(_chunk_size + 2);
      _chunk_size = -1;
    } else {
      break;
    }
  }
  return n_read;
}

int CgiChunkedBody::send(int fd) {
  int n_written;
  if (isChunkedClosed()) {
      if (_len == 0) {
      _len = std::min(_size, _chunked_write_buf.length());
      ft_strlcpy(_buf, _chunked_write_buf.c_str(), _size);
      _chunked_write_buf = _chunked_write_buf.substr(_len);
    }
    if ((n_written = write(fd, _buf, _len)) < 0)
      throw "[CgiChunkedBody::send]: write failed";
    /* not all data of buffer was written */
    if (n_written < _len) {
      _len -= n_written;
      ft_memmove(_buf, _buf + n_written, _len);
      _buf[_len] = '\0';
    }

    if (_chunked_write_buf.length() == 0) {
      log("[CgiChunkedBody::send] all chunked body is sent, closing.\n");
      close(fd);
      Fd::clearWfd(fd);
    }
    return n_written;
  }

  return 0;
}

size_t  CgiChunkedBody::getChunkedContentLength() const {
  return _chunked_write_buf.length();
}