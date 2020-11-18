/* Copyright 2020 hyeyoo */

#include <fcntl.h>
#include <unistd.h>
#include "Config.hpp"
#include "tokenize.hpp"
#include "libft.h"

Config* Config::_instance = nullptr;

Config::Config(const char* path) {
  int ret;
  int fd;
  char *line;

  fd = open(path, O_RDONLY);
  if (fd < 0)
    throw std::exception();

  while ((ret = get_next_line(fd, &line)) > 0) {
    this->_text.push_back(std::string(line));
    free(line);
  }
  if (ret < 0)
    throw std::exception();
  parse();
  validate();
}

void Config::createInstance(const char* path) {
    if (Config::_instance == nullptr)
        Config::_instance = new Config(path);
}

Config* Config::getInstance() {
    if (Config::_instance == nullptr)
        throw std::exception();
    return Config::_instance;
}

void Config::parse() {
  std::vector<std::string> token;

  for (size_t i = 0; i < this->_text.size(); i++) {
    tokenize(this->_text[i], token);
    /* skip empty line */
    if (token.size() <= 0)
      continue;
    std::string key = token[0];
    if (key.compare("server") == 0) {
        assert_token_size(token.size(), 2);
        Server server(token[1].c_str());
        this->_servers.push_back(server);
    }
    token.clear();
  }
  /* validation */
  validate(); 
}

void Config::validate() {
    if (this->_servers.size() == 0)
        throw std::exception();
}

std::vector<Server> const& Config::getServers() const {
    return this->_servers;
}
