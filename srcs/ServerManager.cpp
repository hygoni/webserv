#include <string.h>
#include <iostream>
#include <errno.h>
#include "ServerManager.hpp"
#include "Response.hpp"
#include "Fd.hpp"
#include "debug.hpp"

// request는 closed 됬는데 요청이 계속 들어오면 어떻게 해야하지? recv로 버퍼를 청소해야 하나? 아니면 그냥 냅두나?
ServerManager::ServerManager(Config& config) : _servers(config.getServers()) {
}

ServerManager::~ServerManager() {
}

void displayClients(std::vector<Client*> &clients) {
  std::vector<Client*>::iterator  c_it;

  log("clients: ");
  for (c_it = clients.begin(); c_it != clients.end(); c_it++) {
    log("%d, ", (*c_it)->id);
  }
  log("\n");
}

void  ServerManager::run() {
  std::vector<Server>::iterator   s_it;
  std::vector<Client*>::iterator  c_it;
  struct timeval                  select_timeout;
  fd_set                          all_fds[2], ready_fds[2];

  log("setsize = %d\n", FD_SETSIZE);
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  std::cout.tie(NULL);
  select_timeout.tv_sec = 3;
  Fd::rfds = &all_fds[0];
  Fd::wfds = &all_fds[1];
  ft_bzero(&all_fds, sizeof(fd_set) * 2);
  ft_bzero(&ready_fds, sizeof(fd_set) * 2);
  log("servers initSocket...\n");
  for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
    int server_fd = s_it->initSocket();
    log("server_fd = %d\n", server_fd);
    Fd::set(server_fd, all_fds[0]);
  }
  log("initSocket done\n");
  while (42) {
    ft_bzero(&ready_fds, sizeof(fd_set) * 2);
    ready_fds[0] = all_fds[0];
    ready_fds[1] = all_fds[1];
    if (select(Fd::max_fd + 1, &ready_fds[0], &ready_fds[1], NULL, &select_timeout) < 0) {
      log("max_fd + 1 = %d, strerror(errno) = %s\n", Fd::max_fd + 1, strerror(errno));
      throw "select failed!";
    }
    for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
      if (Fd::isSet(s_it->getFd(), ready_fds[0])) {
        s_it->accept(all_fds[0]);
        /* not setting read fd? */
      }
      std::vector<Client*> &clients = s_it->getClients();
      for (c_it = clients.begin(); c_it != clients.end();) {
       /* Request closed -> Create Resonse -> Process Response */
        if ((*c_it)->isTimeout()) {
          (*c_it)->timeout();
        }

        if ((*c_it)->recv(ready_fds[0]) < 0 || (*c_it)->isConnectionClosed()) {
          Client *client = *c_it;
          c_it = clients.erase(c_it);
          delete client;
          continue ;
        }
        
	/* response exists and ready to write */
        if ((*c_it)->getResponse() != NULL && Fd::isSet((*c_it)->getFd(), ready_fds[1])) {
          (*c_it)->getResponse()->recv(ready_fds[0], ready_fds[1]);
          if ((*c_it)->getResponse()->send((*c_it)->getFd()) < 0) {
            (*c_it)->clear();
            c_it = std::next(c_it);
            continue ;
          }
        }

        c_it = std::next(c_it);
      }
    }
  }
}
