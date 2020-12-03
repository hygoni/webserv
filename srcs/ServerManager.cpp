#include "ServerManager.hpp"
#include "Fd.hpp"

// request는 closed 됬는데 요청이 계속 들어오면 어떻게 해야하지? recv로 버퍼를 청소해야 하나? 아니면 그냥 냅두나?
ServerManager::ServerManager(Config const & config) {
  _servers = config.getServers();
}

ServerManager::~ServerManager() {
}

void  ServerManager::run() {
  std::vector<Server>::iterator s_it;
  std::vector<Client>::iterator c_it;
  std::vector<Client> clients;
  int       max_fd, client_fd;
  fd_set    all_fds[2], ready_fds[2];

  ft_bzero(&all_fds, sizeof(fd_set) * 2);
  for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
    max_fd = s_it->initSocket();
    Fd::set(max_fd, all_fds[0]);
  }

  while (42) {
    ready_fds[0] = all_fds[0];
    ready_fds[1] = all_fds[1];
    if (select(max_fd + 1, &ready_fds[0], &ready_fds[1], NULL, NULL) < 0)
      throw std::exception();
    for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
      if (Fd::isSet(s_it->getFd(), ready_fds[0])) {
        client_fd = s_it->accept(all_fds[0]);
        if (client_fd > max_fd)
          max_fd = client_fd;
      }
      clients = s_it->getClients();
      for (c_it = clients.begin(); c_it != clients.end(); c_it = std::next(c_it)) {
        if (Fd::isSet(c_it->getFd(), ready_fds[0])) {
          c_it->recv();
        }
      }
    }
  }
}
