#include "Socket.hpp"

Socket::Socket() {
  _fd = -1;
};

Socket::~Socket() {
};

bool  Socket::isSet(fd_set const & fds) {
  return fds.fds_bits[_fd / 32] & (1 << (_fd % 32));
}

void  Socket::set(fd_set & fds) {
  FD_SET(_fd, &fds);
}
