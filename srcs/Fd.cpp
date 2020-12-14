#include "Fd.hpp"

fd_set  *Fd::rfds = NULL;
fd_set  *Fd::wfds = NULL;
int     Fd::max_fd = 0;

bool Fd::isSet(int fd, fd_set const & fds) {
  return fds.fds_bits[fd / 32] & (1 << (fd % 32));
}

void  Fd::set(int fd, fd_set & fds) {
  fds.fds_bits[fd / 32] |= (1 << (fd % 32));
  if (fd > max_fd)
    max_fd = fd;
}

void  Fd::clear(int fd, fd_set & fds) {
  fds.fds_bits[fd / 32] = (0 << (fd % 32));
}

void    Fd::clearRfd(int fd) {
  clear(fd, *rfds);
}

void    Fd::clearWfd(int fd) {
  clear(fd, *wfds);
}

void  Fd::setWfd(int fd) {
  Fd::set(fd, *wfds);
}

void  Fd::setRfd(int fd) {
  Fd::set(fd, *rfds);
}