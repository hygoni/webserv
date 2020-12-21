#ifndef SRCS_CGI_HPP_
#define SRCS_CGI_HPP_

/* Copyright 2020 hyeyoo */

#include <map>
#include <sys/wait.h>
#include <string>
#include "Response.hpp"

class Cgi {
private:
  char**  _env;

  Cgi();
  char**  generate_env(std::map<std::string, std::string> const& env_map);
public:
  Cgi(Server const& server, Header const& header);
  ~Cgi();

  void  run(const char* cgi_path, int* request_pipe, int* response_pipe);
};
#endif  // SRCS_CGI_HPP_
