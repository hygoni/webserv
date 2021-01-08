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
  int                             body_write_fd;
  
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  std::cout.tie(NULL);
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
    // usleep(10000);
    ready_fds[0] = all_fds[0];
    ready_fds[1] = all_fds[1];
    /* display rfds before select */
    // Fd::displayFdSet(*Fd::rfds);
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
      displayClients(clients);
      for (c_it = clients.begin(); c_it != clients.end();) {

        /* Request closed -> Create Resonse -> Process Response */

        if ((*c_it)->isTimeout()) {
          (*c_it)->timeout();
       //   log("[Client::timeout]\n");
        } else if (Fd::isSet((*c_it)->getFd(), ready_fds[0])) {
       //   log("[Client::recv]\n");
          (*c_it)->recv();
        }

        /* flsuh buffer */
        /* response read it */
        if ((*c_it)->getRequest() != NULL && (*c_it)->getRequest()->getMethod() == "PUT")
          body_write_fd = (*c_it)->getResponse()->getFileFd();
        else
          body_write_fd = (*c_it)->getRequestPipe()[1];
        if (Fd::isSet(body_write_fd, ready_fds[1])) {
        //  log("[Client::send]\n");
          (*c_it)->send(body_write_fd);
        }

        /* put response to buffer */
        /* response write it */
        int response_read_fd = (*c_it)->getResponsePipe()[0];
        if (Fd::isSet(response_read_fd, ready_fds[0])) {
          //log("[Response::recv]\n");
          (*c_it)->getResponse()->recv(response_read_fd);
        }
        /* flush buffer */
        if (Fd::isSet((*c_it)->getFd(), ready_fds[1])) {
          //log("[Response::send]\n");
          if ((*c_it)->getResponse()->send((*c_it)->getFd()) < 0) {
            Client *client = *c_it;
            c_it = clients.erase(c_it);
            delete client;
            continue ;
          }
        }
        c_it = std::next(c_it);
      }
    }
  }
}
