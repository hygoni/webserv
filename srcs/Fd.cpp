#include "Fd.hpp"

bool Fd::isSet(int fd, fd_set const & fds) {
  return fds.fds_bits[fd / 32] & (1 << (fd % 32));
}

void  Fd::set(int fd, fd_set & fds) {
  fds.fds_bits[fd / 32] |= (1 << (fd % 32));
}
