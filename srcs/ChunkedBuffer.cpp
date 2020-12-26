#include <stdlib.h>
#include <exception>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "ChunkedBuffer.hpp"
#include "libft.h"
#include "Fd.hpp"

/* Copyright 2020 hyeyoo */

ChunkedBuffer::ChunkedBuffer(size_t size) : Buffer(size) {
  _chunk_size = -1;
}

ChunkedBuffer::~ChunkedBuffer() {
  
}

bool ChunkedBuffer::isEmpty() const {
  return _len == 0;
}
/*
 * save size,
 * when size bytes are saved, flsuh by send()
 */

bool  ChunkedBuffer::isHex(char c) {
  if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
    return true;
  return false;
}

bool ChunkedBuffer::isClosed() const {
  return (_chunk_size == 0 && _chunked_write_buf.length() == 0);
}

int ChunkedBuffer::recv(int fd) {
  int     n_read;
  size_t  chunk_size = 0;
  size_t  i;

  /* when buffer isn't empty, don't receive */
  if (!isEmpty()) {
    return 0;
  }

  if ((n_read = read(fd, _buf, _size)) < 0)
    throw "[ChunkedBuffer::send]: read failed";
  _buf[n_read] = '\0';
  _chunked_read_buf += _buf;
  _len = n_read;

  /* if there's no size, parse size */
  if (_chunk_size == -1) {
    for (i = 0; i < _chunked_read_buf.length() && isHex(_chunked_read_buf.at(i)); i++) {
      chunk_size *= 16;
      if (ft_isdigit(_chunked_read_buf.at(i)))
        chunk_size += _chunked_read_buf.at(i) - '0';
      else
        chunk_size += ft_tolower(_chunked_read_buf.at(i)) - 'a' + 16;
    }

    /* if no \r\n after size */
    if (i == 0 || (_chunked_read_buf.at(i) != '\r' && _chunked_read_buf.at(i + 1) != '\n'))
      throw "[ChunkedBuffer::recv] Invalid Chunked Body";
    _chunked_read_buf = _chunked_read_buf.substr(i + 2);
    _chunk_size = chunk_size;

    /* parsed size, but not read chunk-data yet */
  } else if (_chunked_write_buf.length() == 0) {
    if (_chunked_read_buf.length() >= _chunk_size + 2) {
      _chunked_write_buf = _chunked_read_buf.substr(0, _chunk_size);
      if (_chunked_read_buf.substr(_chunk_size, 2).compare("\r\n") != 0)
        throw "[ChunkedBuffer::recv] Invalid Chunked Body";
      _chunked_read_buf = _chunked_read_buf.substr(_chunk_size + 2);
    }
  }
  return n_read;
}

int ChunkedBuffer::send(int fd) {
  int n_written;

  if (_len == 0) {
    int n_buffer = std::min(_size, _chunked_write_buf.length());
    ft_strlcpy(_buf, _chunked_write_buf.c_str(), n_buffer);
    _chunked_write_buf = _chunked_write_buf.substr(n_buffer);
    /* all data of current chunk is written */
    if (_chunked_write_buf.length() == 0 && _chunk_size != 0) {
      _chunk_size = -1;
    }
  }

  if ((n_written = write(fd, _buf, _len)) < 0)
    throw "[ChunkedBuffer::send]: write failed";
  /* not all data of buffer was written */
  if (n_written < _len) {
    _len -= n_written;
    ft_memmove(_buf, _buf + n_written, _len);
    _buf[_len] = '\0';
  }
  return n_written;
}
