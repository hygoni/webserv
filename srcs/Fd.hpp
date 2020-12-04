#ifndef SRCS_SOCKET_HPP_
# define SRCS_SOCKET_HPP_
# include <sys/socket.h>

namespace Fd {
  bool    isSet(int fd, fd_set const & fds);
  void    set(int fd, fd_set& fds);
  void    clear(int fd, fd_set& fds);
}
#endif
