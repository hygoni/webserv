/* Copyright 2020 hyeyoo, hyekim */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <string>

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <string>
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

#define PORT 8006
#define MAX_HEADER_SIZE 8192

int    main(void) {
  int server_fd, client_fd, fd_num;
  int backlog = 10; /* TODO: read from config */
  struct sockaddr_in server_addr, client_addr;
  fd_set current_fds;
  fd_set ready_fds;
  socklen_t addr_len;
  int option = 1;
  int max_fd;
  Request     *requests[FD_SETSIZE] = {NULL, };
  std::string raw_requests[FD_SETSIZE]; /*TODO: need init */
  char read_buf[BUFSIZE];

  /* load config */
  Config::createInstance("./config/config");
  Config* config = Config::getInstance();
  Server const& server = config->getServers()[0];
  (void)config;
 
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cout << strerror(errno) << std::endl;
    return 1;
  }
  /* reuse address */
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  /* TODO: manage multiple servers */
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  std::cout << "port: " << PORT << std::endl;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(&(server_addr.sin_zero), 0, 8);
  if (bind(server_fd, (struct sockaddr *)&server_addr,
      sizeof(struct sockaddr)) < 0) {
    std::cout << strerror(errno) << std::endl;
    return 1;
  }
  listen(server_fd, backlog);

  FD_ZERO(&current_fds);
  FD_SET(server_fd, &current_fds);
  max_fd = server_fd;
  while (42) {
    ready_fds = current_fds;
    std::cout << "selecting..." << std::endl;
    if ((fd_num = select(max_fd + 1, &ready_fds, NULL, NULL, NULL)) < 0) {
      std::cout << strerror(errno) << std::endl;
      return 1;
    }
    /* accept new sockets if fd is set */
    if (FD_ISSET(server_fd, &ready_fds)) {
      addr_len = sizeof(client_addr);
      client_fd = \
        accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
      FD_SET(client_fd, &current_fds);
      if (max_fd < client_fd)
        max_fd = client_fd;
      continue;
    }
    for (int fd = 3; fd <= max_fd; fd++) {
      int n_read;
      if (FD_ISSET(fd, &ready_fds)) {
        if ((n_read = recv(fd, read_buf, BUFSIZE - 1, 0)) > 0) {
          read_buf[n_read] = '\0';
          raw_requests[fd] += read_buf;
          /* make new request instance */
          if (requests[fd] == NULL) {
            size_t header_end = raw_requests[fd].find("\r\n\r\n");
            if (header_end == std::string::npos) {
              if (raw_requests[fd].size() <= MAX_HEADER_SIZE) {
                continue ;
              }
              raw_requests[fd].clear();
              close(fd);
              FD_CLR(fd, &current_fds);
            } else {
              /* detect oversized header */
              if (header_end <= MAX_HEADER_SIZE) {
                try {
                  requests[fd] = new Request(raw_requests[fd]);
                  std::cout << *requests[fd] << std::endl;
                } catch (HttpException & err) {
                  std::cout << "exception:" << err.getStatus() << std::endl;
                  Response response(err.getStatus());
                  response.send(fd);
                  delete requests[fd];
                  requests[fd] = NULL;
                  raw_requests[fd].clear();
                  close(fd);
                  FD_CLR(fd, &current_fds);
                  continue ;
                }
              } else {
                close(fd);
                FD_CLR(fd, &current_fds);
              }
              raw_requests[fd].clear();
            }
          } else {
            /* append on existing request */
            requests[fd]->addBody(raw_requests[fd]);
            raw_requests[fd].clear();
            /* 오류나면 requests[fd]는 delete, set NULL; */
            /* raw_requests[fd].clear(); */
          }
          /* request is closed */
          if (requests[fd]->isClosed()) {
            /* TODO: add response generation and send */
            try {
              Response response(*requests[fd], server);
              response.send(fd);
            } catch (std::exception const& e) {
              std::cout << "error occurred generating response" << std::endl;
            }
            delete requests[fd];
            requests[fd] = NULL;
            raw_requests[fd].clear();
            close(fd);
            FD_CLR(fd, &current_fds);
          }
        } else {
          std::cout << "connection is unexpectedly closed" << std::endl;
          std::cout << "or error occurred while reading" << std::endl;
          if (requests[fd] != NULL)
            delete requests[fd];
          raw_requests[fd].clear();
          close(fd);
          FD_CLR(fd, &current_fds);
        }
      }
    }
  }
  return 0;
}
