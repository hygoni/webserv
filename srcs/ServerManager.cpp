#include "ServerManager.hpp"

ServerManager::ServerManager(Config const & config) {
  _servers = config.getServers();
}

ServerManager::~ServerManager() {
}

int   isSetFd(int n, const struct fd_set *p) {
  return p->fds_bits[n / 32] & (1 << (n % 32));
}

void  ServerManager::run() {
  std::vector<Server>::iterator s_it;
  std::vector<Client>::iterator c_it;
  int       max_fd, client_fd;
  fd_set    read_fds, write_fds, ready_read_fds, ready_write_fds;

  ft_bzero(&read_fds, sizeof(fd_set));
  ft_bzero(&write_fds, sizeof(fd_set));
  for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
    max_fd = s_it->initSocket();
    FD_SET(max_fd, &read_fds);
  }
  while (42) {
    ready_read_fds = read_fds;
    ready_write_fds = write_fds;
    if (select(max_fd + 1, &ready_write_fds, &ready_write_fds, NULL, NULL) < 0) {
      throw std::exception();
    }
    for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {

      if (isSetFd(s_it->getFd(), &ready_write_fds)) {
        client_fd = s_it->accept();
        FD_SET(client_fd, &read_fds);
        if (client_fd > max_fd)
          max_fd = client_fd;
      }
    }
  }
}
