/* Copyright 2020 hyeyoo */
#include <iostream>
#include <unistd.h>
#include <string>
#include <map>
#include "CGI.hpp"
#include "Response.hpp"
#include "libft.h"
#include "Client.hpp"
#include "Fd.hpp"

#define BUFSIZE 4096

Cgi::Cgi(Server const& server, Header const& header) {
  std::map<std::string, std::string>  env_map;
  size_t                              query_idx;
  // env_map["AUTH_TYPE"] = 
  env_map["CONTENT_LENGTH"] =  header["Content-Length"];
  env_map["CONTENT_TYPE"] = header["Content-Type"];
  env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  env_map["PATH_INFO"] = "/";
  // env_map["PATH_TRANSLATED"]
  env_map["QUERY_STRING"] = "";
  if (query_idx = header.getTarget().find('?') != std::string::npos && query_idx != header.getTarget().length())
    env_map["QUERY_STRING"] = header.getTarget().substr(query_idx + 1);
  // env_map["REMOTE_ADDR"]
  // env_map["REMOTE_HOST"]
  // env_map["REMOTE_IDENT"]
  // env_map["REMOTE_USER"]
  env_map["REQUEST_METHOD"] = header.getMethod();
  // env_map["SCRIPT_NAME"]
  env_map["SERVER_NAME"] = server.getServerName();
  env_map["SERVER_PORT"] = server.getListen();
  env_map["SERVER_PROTOCOL"] = header.getVersion();
  // env_map["SERVER_SOFTWARE"]

  if ((_env = generate_env(env_map)) == NULL)
    throw "[Cgi::Cgi]: generating env_map failed due to memory";
}

Cgi::~Cgi() {
  if (_env != NULL)
    ft_free_null_terminated_array(reinterpret_cast<void **>(_env));
}

char **Cgi::generate_env(std::map<std::string, std::string> const &env_map) {
  char **env;
  int i;
  std::map<std::string, std::string>::const_iterator it;

  env = reinterpret_cast<char **>(ft_calloc(sizeof(char *), env_map.size() + 1));
  if (env == NULL) {
    ft_free_null_terminated_array(reinterpret_cast<void **>(env));
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
      ft_free_null_terminated_array(reinterpret_cast<void **>(env));
      return NULL;
    }
    it++;
    i++;
  }
  return env;
}

void Cgi::run(const char *cgi_path, int* request_pipe, int* response_pipe) {
  pid_t pid;
  char *dup;
  char *argv[] = {(dup = ft_strdup(cgi_path)), NULL};

  pipe(response_pipe);
  Fd::setRfd(response_pipe[0]);
  pid = fork();
  if (pid == 0) {
    /* redirect body to stdin */
    dup2(request_pipe[0], 0);
    close(request_pipe[0]);
    close(request_pipe[1]);
    
    /* redirect stdout to response */
    dup2(response_pipe[1], 1);
    close(response_pipe[1]);
    close(response_pipe[0]);
    
    if (execve(cgi_path, argv, _env) < 0)
     throw "[Cgi::run]: execve failed";
    exit(EXIT_SUCCESS);
  } else {
    free(dup);
    close(response_pipe[1]);
    //client.getResponse()->setCgiPid(pid);
  }
}
