/* Copyright 2020 hyeyoo, hyekim */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <string>

#define BUFSIZE 4096

int    main(void) {
    int server_fd, client_fd, fd_num;
    int backlog = 10; /* TODO: read from config */
    struct sockaddr_in server_addr, client_addr;
    fd_set current_fds;
    fd_set ready_fds;
    socklen_t addr_len;
    int max_fd;
    std::string buf[FD_SETSIZE];
    char read_buf[BUFSIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << strerror(errno) << std::endl;
        return 1;
    }
    server_addr.sin_family = AF_INET;
    /* TODO: read from config */
    server_addr.sin_port = htons(8080);
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
            std::cout << "hello accept" << std::endl;
            client_fd = \
                accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
            FD_SET(client_fd, &current_fds);
            if (max_fd < client_fd)
                max_fd = client_fd;
            continue;
        }
        for (int fd = 3; fd <= max_fd; fd++) {
            int n_read;
            std::cout << "reading from " << fd << "..." << std::endl;
            if (FD_ISSET(fd, &ready_fds)) {
                /* when reading request is done */
                if ((n_read = recv(fd, read_buf, BUFSIZE - 1, 0)) == 0) {
                    close(fd);
                    FD_CLR(fd, &current_fds);
                    std::cout << buf[fd] << std::endl;
                    buf[fd].clear();
                } else {
                    read_buf[n_read] = '\0';
                    buf[fd] += std::string(read_buf);
                    std::string response = "HTTP/1.1 200 OK\n"
                    "Date: Sun, 10 Oct 2010 23:26:07 GMT\n"
                    "Server: Apache/2.2.8 (Ubuntu) "
                    "mod_ssl/2.2.8 OpenSSL/0.9.8g\n"
                    "Last-Modified: Sun, 26 Sep 2010 22:04:35 GMT\n"
                    "ETag: \"45b6-834-49130cc1182c0\""
                    "\nAccept-Ranges: bytes"
                    "\nContent-Length: 12\n"
                    "Connection: close\n"
                    "Content-Type: text/html\n"
                    "\nHello world!";
                    send(fd, response.c_str(), response.size(), 0);
                }
            }
        }
    }
    return 0;
}
