#include <string.h>
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

void  ServerManager::run() {
  std::vector<Server>::iterator   s_it;
  std::vector<Client*>::iterator  c_it;
  struct timeval                  select_timeout;
  int       client_fd;
  fd_set    all_fds[2], ready_fds[2];
  
  select_timeout.tv_sec = 3;
  Fd::rfds = &all_fds[0];
  Fd::wfds = &all_fds[1];
  ft_bzero(&all_fds, sizeof(fd_set) * 2);
  log("servers initSocket...\n");
  for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
    int server_fd = s_it->initSocket();
    log("server_fd = %d\n", server_fd);
    Fd::set(server_fd, all_fds[0]);
  }
  log("initSocket done\n");
  while (42) {
    #ifdef DEBUG
      usleep(1000 * 50);
    #endif
    ready_fds[0] = all_fds[0];
    ready_fds[1] = all_fds[1];
    log("select...\n");
    if (select(Fd::max_fd + 1, &ready_fds[0], &ready_fds[1], NULL, &select_timeout) < 0) {
      log("max_fd + 1 = %d, strerror(errno) = %s\n", Fd::max_fd + 1, strerror(errno));
      throw "select failed!";
    }
    for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
      if (Fd::isSet(s_it->getFd(), ready_fds[0])) {
        client_fd = s_it->accept(all_fds[0]);
        /* not setting read fd? */
      }
      std::vector<Client*> &clients = s_it->getClients();
      for (c_it = clients.begin(); c_it != clients.end();) {
        if ((*c_it)->isTimeout()) {
          (*c_it)->timeout();
          log("[Client::tiemout]\n");
        } else if (Fd::isSet((*c_it)->getFd(), ready_fds[0])) {
          log("[Client::recv]\n");
          if ((*c_it)->recv(all_fds[1]) < 0) {
            Client *client = *c_it;
            c_it = clients.erase(c_it);
            delete client;
            continue;
          }
        }
        /* flsuh buffer */
        /* response read it */
        int body_write_fd = (*c_it)->getRequestPipe()[1];
        if (Fd::isSet(body_write_fd, ready_fds[1])) {
          log("[Client::send]\n");
          (*c_it)->send();
        }

        /* put response to buffer */
        /* response write it */
        int response_read_fd = (*c_it)->getResponsePipe()[0];
        if (Fd::isSet(response_read_fd, ready_fds[0])) {
          log("[Response::recv]\n");
          (*c_it)->getResponse()->recv(response_read_fd);
          Fd::setWfd((*c_it)->getFd());
        }
        /* flush buffer */
        if (Fd::isSet((*c_it)->getFd(), ready_fds[1])) {
          log("[Response::send]\n");
          if ((*c_it)->getResponse()->send((*c_it)->getFd()) < 0) {
            Client *client = *c_it;
            c_it = clients.erase(c_it);
            delete client;
            continue;
          }
        }
        c_it = std::next(c_it);
      }
    }
  }
}
