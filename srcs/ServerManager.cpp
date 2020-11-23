#include "ServerManager.hpp"

ServerManager::ServerManager(Config const & config) {
  _servers = config.getServers();
}

ServerManager::~ServerManager() {
}

void  ServerManager::run() {
  std::vector<Server>::iterator s_it;
  std::vector<Client>::iterator c_it;
  int       max_fd, client_fd;
  fd_set    all_fds[2], ready_fds[2];

  ft_bzero(&all_fds, sizeof(fd_set) * 2);
  for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
    max_fd = s_it->initSocket();
    s_it->set(all_fds[0]);
  }

  while (42) {
    ready_fds[0] = all_fds[0];
    ready_fds[1] = all_fds[1];
    if (select(max_fd + 1, &ready_fds[0], &ready_fds[1], NULL, NULL) < 0)
      throw std::exception();
    for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
      if (c_it->isSet(ready_fds[0])) {
        client_fd = s_it->accept();
        c_it->set(all_fds[0]);
        if (client_fd > max_fd)
          max_fd = client_fd;
      }
    }
  }
}
