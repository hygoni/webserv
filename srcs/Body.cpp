#include "Body.hpp"
#include "debug.hpp"
#include <algorithm>

char *Body::_read_buf = (char*)malloc(sizeof(char) * (BUFSIZE + 1));

Body::Body() {

}

Body::Body(std::string const& s) {
  _buf.append(s);
}

Body::~Body() {

}

/* TODO: process transfer encoding */
int Body::recv(int fd) {
  int n_read;
  /* when buffer isn't empty, don't receive */
  if ((n_read = read(fd, _read_buf, BUFSIZE)) < 0) {
    log("[Body::redv] read failed");
    return -1;
  }
  _read_buf[n_read] = '\0';
  _buf.append(_read_buf);
  return n_read;
}

int Body::send(int fd) {
  int n_written;
  int len = std::min(BUFSIZE, (int)_buf.length());
  if ((n_written = write(fd, _buf.c_str(), len)) < 0) {
    log("[Body::send] read failed");
    return -1;
  }
  /* not all data of buffer was written */
  _buf.erase(0, n_written);
  return n_written;
}

int  Body::getChunkedContentLength() const {
  return 0;
}

bool    Body::isChunkedReceived() const {
  return false;
}

bool    Body::isChunkedSent() const {
  return false;
}