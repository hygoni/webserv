#ifndef SRCS_FD_HPP_
# define SRCS_FD_HPP_
# include <sys/socket.h>
# include <stdlib.h>
# include "debug.hpp"

# ifdef __APPLE__
#  define MY_FD_MASK    int32_t
# elif __linux__
#  define MY_FD_MASK    long int
# else
#  define MY_FD_MASK    int32_t
# endif

# define MY_NBBY        8
# define MY_NFDBITS     (MY_NBBY * sizeof(MY_FD_MASK))

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
