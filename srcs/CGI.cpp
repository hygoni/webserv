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

std::string ip_to_string(int ip) {
  std::string ip_string;

  while (ip > 0) {
    ip_string = std::to_string(ip % 256) + ip_string;
    ip /= 256;
    if (ip != 0) {
      ip_string = "." + ip_string;
    }
  }
  return ip_string;
}

Cgi::Cgi(Client& client) : _client(client) {
  std::map<std::string, std::string>  env_map;
  size_t                              idx;
  Server const&                       server = client.getServer();
  struct sockaddr_in                  client_addr;
  socklen_t                           client_len;
  Header&                             header = *(client.getRequest()->getHeader());
  std::string                         cgi_path;
  std::string                         cgi_file_path = "/";
  std::string                         query;

  client_len = sizeof(client_addr);
  getsockname(client.getFd(), (struct sockaddr*)&client_addr, &client_len);
  
  /* get query */
  idx = header.getTarget().find('?');
  if (idx != std::string::npos)
    query = header.getTarget().substr(idx + 1);
  cgi_path = header.getTarget().substr(client.getLocation()->getPath().length());
  idx = cgi_path.find('/');
  if (idx != std::string::npos) {
    cgi_file_path = cgi_path.substr(idx);
    cgi_path = client.getLocation()->getRoot() + cgi_path.substr(0, idx);
  }

  // env_map["AUTH_TYPE"] = 
  env_map["CONTENT_LENGTH"] =  header["Content-Length"];
  env_map["CONTENT_TYPE"] = header["Content-Type"];
  env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  env_map["PATH_INFO"] = cgi_file_path ;
  env_map["PATH_TRANSLATED"] = client.getLocation()->getRoot() + "/" + cgi_file_path;
  env_map["QUERY_STRING"] = query;
  env_map["REMOTE_ADDR"] = ip_to_string(client_addr.sin_addr.s_addr);
  // env_map["REMOTE_IDENT"]
  // env_map["REMOTE_USER"]
  env_map["REQUEST_METHOD"] = header.getMethod();
  env_map["REQUEST_URI"] = header.getTarget();
  env_map["SCRIPT_NAME"] = cgi_path;
  env_map["SERVER_NAME"] = server.getServerName();
  env_map["SERVER_PORT"] = std::to_string(server.getListen());
  env_map["SERVER_PROTOCOL"] = header.getVersion();
  env_map["REDIRECT_STATUS"] = "0";
  env_map["SERVER_SOFTWARE"] = "webserv/1.0";
  env_map["SCRIPT_FILENAME"] = client.getLocation()->getRoot() + "/" + cgi_file_path;

  if ((_env = generate_env(env_map)) == NULL)
    throw "[Cgi::Cgi]: generating env_map failed due to memory";
  _cgi_path = cgi_path;
  _cgi_file_path = cgi_file_path;
}

Cgi::~Cgi() {
  log("[Cgi::~Cgi]\n");
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

void Cgi::run() {
  pid_t pid;
  char *dup_cgi_path;
  char *dup_file_path;
  char* const argv[] = {dup_cgi_path = ft_strdup(_cgi_path.c_str()),
  dup_file_path = ft_strdup(_cgi_file_path.c_str()), NULL};
  
  pipe(_client.getResponsePipe());
  Fd::setRfd(_client.getResponsePipe()[0]);
  pid = fork();
  if (pid == 0) {
    log("[Cgi::run] argv[0] = %s, argv[1] = %s, argv[2] = %s", argv[0], argv[1], argv[2]);
    std::cout << std::endl;
    /* redirect body to stdin */
    dup2(_client.getRequestPipe()[0], 0);
    close(_client.getRequestPipe()[0]);
    close(_client.getRequestPipe()[1]);
    
    /* redirect stdout to response */
    dup2(_client.getResponsePipe()[1], 1);
    close(_client.getResponsePipe()[1]);
    close(_client.getResponsePipe()[0]);
    if (execve(_cgi_path.c_str(), argv, _env) < 0)
     throw "[Cgi::run]: execve failed";
    free(dup_cgi_path);
    free(dup_file_path);
    exit(EXIT_SUCCESS);
  } else {
    close(_client.getResponsePipe()[1]);
    free(dup_cgi_path);
    free(dup_file_path);
    //_client.getResponse()->setCgiPid(pid);
  }
}
