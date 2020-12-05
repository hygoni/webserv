/* Copyright 2020 hyeyoo */
#include <iostream>
#include <unistd.h>
#include <string>
#include <map>
#include "CGI.hpp"
#include "Response.hpp"
#include "libft.h"
#include "Client.hpp"

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
(Client& client,
const char* cgi_path,
std::map<std::string, std::string> const& env_map,
int body_read_fd) {
    pid_t pid;
    char *dup;
    char **env;
    char *argv[] = {(dup = ft_strdup(cgi_path)), NULL};

    if ((env = generate_env(env_map)) == NULL) {
      free(dup);
      throw std::exception();
    }
    pipe(client.getResponsePipe());
    pid = fork();
    if (pid == 0) {
        /* redirect body to stdin */
        dup2(body_read_fd, 0);
        close(body_read_fd);
        /* redirect stdout to response */
        dup2(client.getResponsePipe()[1], 1);
        close(client.getResponsePipe()[0]);
        close(client.getResponsePipe()[1]);
        if (execve(cgi_path, argv, env) < 0)
            throw std::exception();
    } else {
        free(dup);
        ft_free_null_terminated_array(reinterpret_cast<void**>(env));
        close(client.getResponsePipe()[1]);
        client.getResponse()->setCgiPid(pid);
    }
}
