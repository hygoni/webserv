#ifndef SRCS_FD_HPP_
# define SRCS_FD_HPP_
# include <sys/socket.h>
# include <stdlib.h>
# include "debug.hpp"

class Fd {
public:
  static fd_set  *rfds;
  static fd_set  *wfds;
  static int     max_fd;
  static bool    isSet(int fd, fd_set const & fds);
  static void    set(int fd, fd_set& fds);
  static void    setWfd(int fd);
  static void    setRfd(int fd);
  static void    clear(int fd, fd_set& fds);
  static void    clearRfd(int fd);
  static void    clearWfd(int fd);
  static void    displayFdSet(fd_set &fds);
  static void    displayBrokenFdSet(fd_set &fds);
  static void    close(int& fd);
};
#endif
