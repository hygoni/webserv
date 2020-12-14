#include <string.h>
#include <errno.h>
#include "ServerManager.hpp"
#include "Response.hpp"
#include "Fd.hpp"

// request는 closed 됬는데 요청이 계속 들어오면 어떻게 해야하지? recv로 버퍼를 청소해야 하나? 아니면 그냥 냅두나?
ServerManager::ServerManager(Config& config) : _servers(config.getServers()) {
}

ServerManager::~ServerManager() {
}

void  ServerManager::run() {
  std::vector<Server>::iterator s_it;
  std::vector<Client>::iterator c_it;
  int       client_fd;
  fd_set    all_fds[2], ready_fds[2];
  
  Fd::rfds = &all_fds[0];
  Fd::wfds = &all_fds[1];
  ft_bzero(&all_fds, sizeof(fd_set) * 2);
  for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
    Fd::set(s_it->initSocket(), all_fds[0]);
  }
  std::cout << "servers initSocket.." << std::endl;
  while (42) {
    ready_fds[0] = all_fds[0];
    ready_fds[1] = all_fds[1];
    std::cout << "select? max_fd: " << Fd::max_fd << std::endl;
      std::cout << strerror(errno) << std::endl;
    if (select(Fd::max_fd + 1, &ready_fds[0], &ready_fds[1], NULL, NULL) < 0) {
      std::cout << (Fd::max_fd + 1) << std::endl;
      std::cout << strerror(errno) << std::endl;
      throw "select failed!";
    }
    std::cout << "select.." << std::endl;
    for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
      if (Fd::isSet(s_it->getFd(), ready_fds[0])) {
        client_fd = s_it->accept(all_fds[0]);
        /* not setting read fd? */
      }
      std::vector<Client> &clients = s_it->getClients();
      for (c_it = clients.begin(); c_it != clients.end(); c_it = std::next(c_it)) {
        /* put request to buffer */
        if (Fd::isSet(c_it->getFd(), ready_fds[0])) {
          c_it->recv(all_fds[1]);
        }
        /* flsuh buffer */
        /* response read it */
        int body_write_fd = c_it->getRequestPipe()[1];
        if (Fd::isSet(body_write_fd, ready_fds[1])) {
          std::cout << "body write fd ready" << std::endl;
          c_it->send();
        }

        /* put response to buffer */
        /* response write it */
        int response_read_fd = c_it->getResponsePipe()[0];
        if (Fd::isSet(response_read_fd, ready_fds[0])) {
          Fd::setWfd(c_it->getFd());
          std::cout << "response read fd is set !" << std::endl;
          c_it->getResponse()->recv(response_read_fd);
        }
        /* flush buffer */
        if (Fd::isSet(c_it->getFd(), ready_fds[1])) {
          std::cout << "response send ready" << std::endl;
          c_it->getResponse()->send(c_it->getFd());
        }
      }
    }
  }
}
