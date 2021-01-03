#include "Fd.hpp"
#include "debug.hpp"
#include <fcntl.h>
#include <unistd.h>

fd_set  *Fd::rfds = NULL;
fd_set  *Fd::wfds = NULL;
int     Fd::max_fd = 0;

bool Fd::isSet(int fd, fd_set const & fds) {
  return fds.fds_bits[fd / 32] & (1 << (fd % 32));
}

void Fd::displayFdSet(fd_set &fds) {
  log("[Fd::displayFdSet] = [");
  for (int i = 3; i <= max_fd; i++) {
    if (isSet(i, fds)) {
      log("%d, ", i);
    }
  }
  log("]\n");
}

void  Fd::set(int fd, fd_set & fds) {
  if (fd < 0) 
    return;
  displayFdSet(fds);
  /// fcntl(fd, F_SETFL, O_NONBLOCK);
  fds.fds_bits[fd / 32] |= (1 << (fd % 32));
  if (fd > max_fd)
    max_fd = fd;
  displayFdSet(fds);
}

void  Fd::clear(int fd, fd_set & fds) {
  if (fd < 0) 
    return;
  displayFdSet(fds);
  fds.fds_bits[fd / 32] &= ~(1 << (fd % 32));
  displayFdSet(fds);
}

void    Fd::clearRfd(int fd) {
  log("[Fd::clearRfd] - ");
  clear(fd, *rfds);
}

void    Fd::clearWfd(int fd) {
  log("[Fd::clearWfd] - ");
  clear(fd, *wfds);
}

void  Fd::setWfd(int fd) {
  log("[Fd::setWfd] - ");
  Fd::set(fd, *wfds);
}

void  Fd::setRfd(int fd) {
  log("[Fd::setRfd] - ");
  Fd::set(fd, *rfds);
}