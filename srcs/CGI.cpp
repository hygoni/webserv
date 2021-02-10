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
#include "debug.hpp"
#include "utils.hpp"

std::string ip_to_string(int ip) {
  std::string ip_string;

  while (ip > 0) {
    ip_string = ip_string + to_string(ip % 256);
    ip /= 256;
    if (ip != 0) {
      ip_string = ip_string + ".";
    }
  }
  return ip_string;
}

Cgi::Cgi(Client& client) : _client(client) {
  std::map<std::string, std::string>  env_map;
  struct sockaddr_in                  client_addr;
  socklen_t                           client_len;
  Header&                             header = *(client.getRequest()->getHeader());
  std::string                         cgi_path;
  std::string                         cgi_file_path = "/";

  _pid = -1;
  client_len = sizeof(client_addr);
  getsockname(client.getFd(), (struct sockaddr*)&client_addr, &client_len);

  cgi_path = client.getCgiPath();
  cgi_file_path = client.getCgiFilePath();

  if (!header["Authorization"].empty()) {
    env_map["AUTH_TYPE"] = "Basic";
    env_map["REMOTE_USER"] = client.getRequest()->getUserName();
  }
  if (!header.isExist("Content-Length"))
    env_map["CONTENT_LENGTH"] = "0";
  else
    env_map["CONTENT_LENGTH"] =  header["Content-Length"];
  env_map["CONTENT_TYPE"] = header["Content-Type"];
  env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  env_map["PATH_INFO"] = header.getTarget();
  env_map["PATH_TRANSLATED"] = client.getLocation()->getRoot() + "/"
  + header.getTarget().substr(client.getLocation()->getPath().length());
  env_map["QUERY_STRING"] = client.getRequest()->getQuery();
  env_map["REMOTE_ADDR"] = ip_to_string(client_addr.sin_addr.s_addr);
  env_map["REQUEST_METHOD"] = header.getMethod();
  env_map["REQUEST_URI"] = header.getTarget();
  env_map["SCRIPT_NAME"] = env_map["PATH_TRANSLATED"];
  env_map["SERVER_NAME"] = client.getServer().getServerName();
  env_map["SERVER_PORT"] = to_string(client.getServer().getListen());
  env_map["SERVER_PROTOCOL"] = header.getVersion();
  env_map["SERVER_SOFTWARE"] = "webserv/1.0";
  env_map["SCRIPT_FILENAME"] = env_map["SCRIPT_NAME"];
  env_map["REDIRECT_STATUS"] = "0";

  std::map<std::string, std::string>::const_iterator it = header.getBegin();
  while (it != header.getEnd()) {
    std::string key = ("HTTP_" + it->first);
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    env_map[key] = it->second;
    it++;
  }
  for (it = env_map.begin(); it != env_map.end(); it++) {
    debug_printf("key : %s, value : %s\n", it->first.c_str(), it->second.c_str());
  }

  if ((_env = generate_env(env_map)) == NULL)
    throw "[Cgi::Cgi]: generating env_map failed due to memory";
  _cgi_path = cgi_path;
  _cgi_file_path = cgi_file_path;
}

Cgi::~Cgi() {
  debug_printf("[Cgi::~Cgi]\n");
  if (_env != NULL)
    ft_free_null_terminated_array(reinterpret_cast<void **>(_env));
  /* prevent zombie process */
  waitpid(_pid, NULL, WNOHANG);
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
  it = env_map.begin();
  while (it != env_map.end()) {
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

  pipe(_client.getRequestPipe());
  Fd::setWfd(_client.getRequestPipe()[1]);
  pipe(_client.getResponsePipe());
  Fd::setRfd(_client.getResponsePipe()[0]);
  pid = fork();
  if (pid == 0) {
    debug_printf("[Cgi::run] argv[0] = %s, argv[1] = %s, argv[2] = %s\n", argv[0], argv[1], argv[2]);
    /* redirect body to stdin */
    if (_client.getRequestPipe()[0] == -1) {
      close(0);
    } else {
      dup2(_client.getRequestPipe()[0], 0);
      close(_client.getRequestPipe()[0]);
      close(_client.getRequestPipe()[1]);
    }

    /* redirect stdout to response */
    dup2(_client.getResponsePipe()[1], 1);
    close(_client.getResponsePipe()[1]);
    close(_client.getResponsePipe()[0]);
    if (execve(_cgi_path.c_str(), argv, _env) < 0)
     exit(EXIT_FAILURE);
    free(dup_cgi_path);
    free(dup_file_path);
    exit(EXIT_SUCCESS);
  } else {
    _pid = pid;
    Fd::close(_client.getResponsePipe()[1]);
    free(dup_cgi_path);
    free(dup_file_path);
    if (isCgiError())
      throw HttpException(500);
  }
}

bool Cgi::isCgiError() const {
  int status;

  if (_pid < 0)
    return false;
  else {
    int ret = waitpid(_pid, &status, WNOHANG);
    bool is_error = WEXITSTATUS(status) == EXIT_FAILURE;
    return (ret > 0 && is_error);
  }
}
