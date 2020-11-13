#ifndef SRCS_CGI_HPP_
#define SRCS_CGI_HPP_

/* Copyright 2020 hyeyoo */

#include <map>
#include <string>

std::string run_cgi
(char* cgi_path, std::map<std::string, std::string> const& env_map);
char        **generate_env(std::map<std::string, std::string> const& env_map);

#endif  // SRCS_CGI_HPP_
