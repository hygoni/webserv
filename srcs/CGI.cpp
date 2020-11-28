/* Copyright 2020 hyeyoo */
#include <iostream>
#include <unistd.h>
#include <string>
#include <map>
#include "CGI.hpp"
#include "libft.h"

#define BUFSIZE 4096

char    **generate_env(std::map<std::string, std::string> const& env_map) {
    char **env;
    int i;
    std::map<std::string, std::string>::const_iterator it;

    env = reinterpret_cast<char**>
        (ft_calloc(sizeof(char*), env_map.size() + 1));
    if (env == NULL) {
        ft_free_null_terminated_array(reinterpret_cast<void**>(env));
        return NULL;
    }

    i = 0;
    it = env_map.cbegin();
    while (it != env_map.cend()) {
        std::string line;
        line += it->first;
        line += "=";
        line += it->second;
        if ((env[i] = ft_strdup(line.c_str())) == NULL) {
            ft_free_null_terminated_array(reinterpret_cast<void**>(env));
            return NULL;
        }
        it++;
        i++;
    }
    return env;
}

void run_cgi
(Response& response,
char* cgi_path,
std::map<std::string, std::string> const& env_map,
int body_read_fd) {
    int response_fd[2];
    pid_t pid;
    char **env;
    char* argv[] = {cgi_path, NULL};

    if ((env = generate_env(env_map)) == NULL)
        throw std::exception();
    pipe(response_fd);
    pid = fork();
    if (pid == 0) {
        /* redirect body to stdin */
        dup2(body_read_fd, 0);
        close(body_read_fd);
        /* redirect stdout to response */
        dup2(response_fd[1], 1);
        close(response_fd[0]);
        close(response_fd[1]);
        if (execve(cgi_path, argv, env) < 0)
            throw std::exception();
    } else {
        ft_free_null_terminated_array(reinterpret_cast<void**>(env));
        close(response_fd[1]);
        response.setResponseReadFd(fd[0]);
        response.setCgiPid(pid);
    }
}
