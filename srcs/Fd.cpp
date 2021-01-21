#include "Fd.hpp"
#include "debug.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <vector>
#include "Client.hpp"
#include "signals.hpp"

fd_set  *Fd::rfds = NULL;
fd_set  *Fd::wfds = NULL;
int     Fd::max_fd = 0;

bool Fd::isSet(int fd, fd_set const & fds) {
  if (g_is_sigpipe)
    return false;
  else
    return FD_ISSET(fd, &fds);
//  return fds.fds_bits[fd / 32] & (1 << (fd % 32));
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

void Fd::close(int& fd) {
  clearRfd(fd);
  clearWfd(fd);
  if (fd < 0)
    return ;
  ::close(fd);
  fd = -1;
}

void  Fd::set(int fd, fd_set & fds) {
  if (fd < 0) 
    return;
  displayFdSet(fds);
  fcntl(fd, F_SETFL, O_NONBLOCK);
  // fds.fds_bits[fd / 32] |= (1 << (fd % 32));
  FD_SET(fd, &fds);
  if (fd > max_fd)
    max_fd = fd;
  displayFdSet(fds);
}

void  Fd::clear(int fd, fd_set & fds) {
  if (fd < 0) 
    return;
  displayFdSet(fds);
  FD_CLR(fd, &fds);
  // fds.fds_bits[fd / 32] &= ~(1 << (fd % 32));
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
