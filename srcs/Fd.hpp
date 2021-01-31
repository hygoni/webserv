#ifndef SRCS_FD_HPP_
# define SRCS_FD_HPP_
# include <sys/socket.h>
# include <stdlib.h>
# include "debug.hpp"

# ifndef __DARWIN_NFDBITS
#  define __DARWIN_NBBY   8       /* bits in a byte */
#  define __DARWIN_NFDBITS  (sizeof(__int32_t) * __DARWIN_NBBY) /* bits per mask */
# endif

class Fd {
public:
  static fd_set  *rfds;
  static fd_set  *wfds;
  static int     max_fd;
  static bool    isSet(int fd, const fd_set *fds);
  static void    set(int fd, fd_set *fds);
  static void    setWfd(int fd);
  static void    setRfd(int fd);
  static void    clear(int fd, fd_set *fds);
  static void    clearRfd(int fd);
  static void    clearWfd(int fd);
  static void    displayFdSet(const fd_set *fds);
  static void    close(int& fd);
};
#endif
