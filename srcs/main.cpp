#include <stdio.h>
#include <unistd.h>
<<<<<<< Updated upstream
#include <fcntl.h>
#include "Server.hpp"

int     main(void)
{
    int fd = open("./config/server", O_RDONLY);
    if (fd < 0)
        return 1;

    Server server(fd);
=======
#include <iostream>
#include "Request.hpp"
#include <cstring>
#include <cerrno>
#include <string>

#define BUFSIZE 8192
#define MAX_HEADER_SIZE 8192
// TODO: get MAX_BODY_SIZE from config
void    send_response_with_status(int status, int fd) {
  std::string response = "HTTP/1.1 ";
  response += std::to_string(status);
  response += " OK\r\nDate: Sun, 10 Oct 2010 23:26:07 GMT\r\n" \
                    "Server: Apache/2.2.8 (Ubuntu)\r\n" \
                    "mod_ssl/2.2.8 OpenSSL/0.9.8g\r\n" \
                    "Last-Modified: Sun, 26 Sep 2010 22:04:35 GMT\r\n" \
                    "Accept-Ranges: bytes\r\n" \
                    "Content-Length: 12\r\n" \
                    "Connection: close\r\n" \
                    "Content-Type: text/html\r\n" \
                    "\r\nHello world!";
  send(fd, response.c_str(), response.size(), 0);
}

int    main(void) {
    int server_fd, client_fd, fd_num;
    int backlog = 10; /* TODO: read from config */
    struct sockaddr_in server_addr, client_addr;
    fd_set current_fds;
    fd_set ready_fds;
    socklen_t addr_len;
    int max_fd;
    std::string raw_requests[FD_SETSIZE]; // TODO: need init
    char read_buf[BUFSIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << strerror(errno) << std::endl;
        return 1;
    }
    server_addr.sin_family = AF_INET;
    /* TODO: read from config */
    server_addr.sin_port = htons(8082);
    /* TODO: read from config */
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
                  std::cout << "n_read: " << n_read << std::endl;
                  read_buf[n_read] = '\0';
                  raw_requests[fd] += read_buf;
                  size_t header_end = raw_requests[fd].find("\r\n\r\n");
                  // raw_requests[fd].clear();
                  // close(fd);
                  // FD_CLR(fd, &current_fds);
                } else {
                    std::cout << "connection is unexpectedly closed" << std::endl;
                    std::cout << "or error occurred while reading" << std::endl;
                    raw_requests[fd].clear();
                    close(fd);
                    FD_CLR(fd, &current_fds);
                }
            }
        }
    }
>>>>>>> Stashed changes
    return 0;
}
