#ifndef SRCS_CONFIG_HPP_
#define SRCS_CONFIG_HPP_

/* Copyright 2020 hyeyoo */

#include <string>
#include <vector>
#include "Server.hpp"
#include "Location.hpp"

/*
 * configuration class
 * uses singleton pattern
 */
class Config {
 private:
     static Config*             _instance;
     std::vector<Server>        _servers;
     std::vector<std::string>   _text;
     explicit                   Config(const char *path);
 public:
     static void                createInstance(const char* path);
     static Config*             getInstance();
     std::vector<Server>&       getServers();
     void                       parse();
     void                       validate();
};

#endif  // SRCS_CONFIG_HPP_
