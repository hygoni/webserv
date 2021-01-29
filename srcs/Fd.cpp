#include "Fd.hpp"
#include "debug.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <vector>
#include "Client.hpp"
#include "signals.hpp"

/*
  reference - https://opensource.apple.com/source/xnu/xnu-3248.20.55/bsd/sys/_types/_fd_def.h
*/

fd_set  *Fd::rfds = NULL;
fd_set  *Fd::wfds = NULL;
int     Fd::max_fd = 0;
extern bool g_is_sigpipe;

bool Fd::isSet(int fd, const fd_set *fds) {
  if (g_is_sigpipe || fd < 0)
    return false;
  else
    return FD_ISSET(fd, fds);
}

void Fd::displayFdSet(const fd_set *fds) {
  debug_printf("[Fd::displayFdSet] = [");
  for (int i = 3; i <= max_fd; i++) {
    if (isSet(i, fds)) {
      debug_printf("%d, ", i);
    }
  }
  debug_printf("]\n");
}

void Fd::close(int& fd) {
  clearRfd(fd);
  clearWfd(fd);
  if (fd < 0)
    return ;
  ::close(fd);
  fd = -1;
}

void  Fd::set(int fd, fd_set *fds) {
  if (fd < 0)
    return;
  displayFdSet(fds);
  fcntl(fd, F_SETFL, O_NONBLOCK);
  fds->fds_bits[(unsigned long)fd /__DARWIN_NFDBITS] |= ((__int32_t)(((unsigned long)1)<<((unsigned long)fd  % __DARWIN_NFDBITS)));
  if (fd > max_fd)
    max_fd = fd;
  displayFdSet(fds);
}

void  Fd::clear(int fd, fd_set *fds) {
  if (fd < 0)
    return;
  displayFdSet(fds);
  fds->fds_bits[(unsigned long)fd/__DARWIN_NFDBITS] &= ~((__int32_t)(((unsigned long)1)<<((unsigned long)fd % __DARWIN_NFDBITS)));
  displayFdSet(fds);
}

void    Fd::clearRfd(int fd) {
  debug_printf("[Fd::clearRfd] - ");
  clear(fd, rfds);
}

void    Fd::clearWfd(int fd) {
  debug_printf("[Fd::clearWfd] - ");
  clear(fd, wfds);
}

void  Fd::setWfd(int fd) {
  debug_printf("[Fd::setWfd] - ");
  Fd::set(fd, wfds);
}

void  Fd::setRfd(int fd) {
  debug_printf("[Fd::setRfd] - ");
  Fd::set(fd, rfds);
}
