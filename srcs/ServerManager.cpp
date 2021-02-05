#include <string.h>
#include <iostream>
#include <signal.h>
#include <errno.h>
#include "ServerManager.hpp"
#include "Response.hpp"
#include "Fd.hpp"
#include "debug.hpp"
#include "signals.hpp"

extern bool g_is_sigpipe;

// request는 closed 됬는데 요청이 계속 들어오면 어떻게 해야하지? recv로 버퍼를 청소해야 하나? 아니면 그냥 냅두나?
ServerManager::ServerManager(Config& config) : _servers(config.getServers()) {
}

ServerManager::~ServerManager() {
}

void displayClients(std::vector<Client*> &clients) {
  std::vector<Client*>::iterator  c_it;

  debug_printf("clients: ");
  for (c_it = clients.begin(); c_it != clients.end(); c_it++) {
    debug_printf("%d, ", (*c_it)->id);
  }
  debug_printf("\n");
}

void  ServerManager::run() {
  std::vector<Server>::iterator   s_it;
  std::vector<Client*>::iterator  c_it;
  struct timeval                  select_timeout;
  fd_set                          all_fds[2], ready_fds[2];

  signal(SIGPIPE, sigpipe_handler);
  debug_printf("setsize = %d\n", FD_SETSIZE);
  select_timeout.tv_sec = 3;
  select_timeout.tv_usec = 0;
  Fd::rfds = &all_fds[0];
  Fd::wfds = &all_fds[1];
  ft_bzero(&all_fds, sizeof(fd_set) * 2);
  ft_bzero(&ready_fds, sizeof(fd_set) * 2);
  debug_printf("servers initSocket...\n");
  for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
    int server_fd = s_it->initSocket();
    debug_printf("server_fd = %d\n", server_fd);
    Fd::set(server_fd, &all_fds[0]);
  }
  debug_printf("initSocket done\n");
  while (42) {
    ft_bzero(&ready_fds, sizeof(fd_set) * 2);
    ready_fds[0] = all_fds[0];
    ready_fds[1] = all_fds[1];
    g_is_sigpipe = false;
    if (select(Fd::max_fd + 1, &ready_fds[0], &ready_fds[1], NULL, &select_timeout) < 0) {
      debug_printf("max_fd + 1 = %d, strerror(errno) = %s\n", Fd::max_fd + 1, strerror(errno));
      throw "select failed!";
    }
    for (s_it = _servers.begin(); s_it != _servers.end(); s_it = std::next(s_it)) {
      if (Fd::isSet(s_it->getFd(), &ready_fds[0])) {
        s_it->accept(&all_fds[0]);
        /* not setting read fd? */
      }
      std::vector<Client*> &clients = s_it->getClients();
      for (c_it = clients.begin(); c_it != clients.end();) {
        try {
          /* Request closed -> Create Resonse -> Process Response */
          if ((*c_it)->isTimeout()) {
            (*c_it)->timeout();
          }

          if ((*c_it)->recv(&ready_fds[0]) < 0 || (*c_it)->isConnectionClosed()) {
            Client *client = *c_it;
            c_it = clients.erase(c_it);
            delete client;
            continue ;
          }

          if ((*c_it)->getResponse() && (*c_it)->getResponse()->getCgi()) {
            debug_printf("[ServerManager::run] is cgi error?\n");
            if ((*c_it)->getResponse()->getCgi()->isCgiError()) {
              debug_printf("[ServerManager::run] throw 500!\n");
              throw HttpException(500);
            }
          }

          /* response exists and ready to write */
          if ((*c_it)->getResponse() != NULL && Fd::isSet((*c_it)->getFd(), &ready_fds[1])) {
            (*c_it)->getResponse()->recv(&ready_fds[0], &ready_fds[1]);
            if ((*c_it)->getResponse()->send((*c_it)->getFd()) < 0) {
              /* if status is not 200 */
              if ((*c_it)->getResponse()->getHeader()->getStatus() / 100 != 2) {
                Client *client = *c_it;
                c_it = clients.erase(c_it);
                delete client;
                continue ;
              } else {
                (*c_it)->clear();
              }
            }
          }
        } catch (const std::exception& e) {
          debug_printf("[ServerManager::run] Internal Error : 500");
          (*c_it)->setResponse(new Response(**c_it, 500));
          Fd::setWfd((*c_it)->getFd());
        }
        c_it = std::next(c_it);
      }
    }
  }
}
