#ifndef SRCS_CGI_HPP_
#define SRCS_CGI_HPP_

/* Copyright 2020 hyeyoo */

#include <map>
#include <sys/wait.h>
#include <string>
#include "Response.hpp"

class Cgi {
private:
  char**          _env;
  Client&          _client;
  std::string     _cgi_path;
  std::string     _cgi_file_path;
  Cgi();
  char**  generate_env(std::map<std::string, std::string> const& env_map);
public:
  Cgi(Client& client);
  ~Cgi();

  void  run();
};
#endif  // SRCS_CGI_HPP_
