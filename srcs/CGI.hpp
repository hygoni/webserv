#ifndef SRCS_CGI_HPP_
#define SRCS_CGI_HPP_

/* Copyright 2020 hyeyoo */

#include <map>
#include <sys/wait.h>
#include <string>
#include "Response.hpp"

void run_cgi
(Client& client,
const char* cgi_path,
std::map<std::string, std::string> const& env_map,
int body_read_fd);

char        **generate_env(std::map<std::string, std::string> const& env_map);

#endif  // SRCS_CGI_HPP_
