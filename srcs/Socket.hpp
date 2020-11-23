#ifndef SRCS_SOCKET_HPP_
# define SRCS_SOCKET_HPP_
# include <sys/socket.h>

class Socket {
protected:
  Socket();
  int     _fd;
public:
  virtual ~Socket();
  bool    isSet(fd_set const & fds);
  void    set(fd_set& fds);
};
#endif
