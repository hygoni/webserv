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

std::string run_cgi
(char* cgi_path, std::map<std::string, std::string> const& env_map) {
    pid_t pid;
    int fd[2];
    char **env;
    char* argv[] = {cgi_path, NULL};
    char read_buf[BUFSIZE + 1];
    std::string     output;

    if ((env = generate_env(env_map)) == NULL)
        throw std::exception();
    pipe(fd);
    pid = fork();
    if (pid == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        if (execve(cgi_path, argv, env) < 0)
            throw std::exception();
    } else {
        int             ret;

        waitpid(pid, NULL, 0);
        close(fd[1]);
        ft_free_null_terminated_array(reinterpret_cast<void**>(env));
        while ((ret = read(fd[0], read_buf, BUFSIZE)) > 0) {
            read_buf[ret] = '\0';
            output += read_buf;
        }
        if (ret < 0)
            throw std::exception();
    }
    return output;
}
