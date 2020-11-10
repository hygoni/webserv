/* Copyright 2020 hyeyoo */

#include <vector>
#include <utility>
#include "libft.h"
#include "tokenize.hpp"
#include "Server.hpp"

/*
 * generate text
 */
Server::Server(int fd) {
  int ret;
  char *line;

  while ((ret = get_next_line(fd, &line)) > 0) {
    this->_text.push_back(std::string(line));
    free(line);
  }
  if (ret < 0)
    throw std::exception();
  parseServer();
  validate();
}

void assert_token_size(int size, int minimum) {
    if (minimum == 0)
        return;
    if (size < minimum)
        throw std::exception();
}

void Server::parseServer() {
  std::vector<std::string> token;
  for (size_t i = 0; i < this->_text.size(); i++) {
    tokenize(this->_text[i], token);
    /* skip empty line */
    if (token.size() <= 0)
      continue;
    std::string key = token[0];
    if (key.compare("listen") == 0 ||
            key.compare("server_name") == 0 ||
            key.compare("root") == 0 ||
            key.compare("path") == 0) {
        assert_token_size(token.size(), 2);
        std::string value = token[1];
        this->setAttribute(key, value);
    } else if (key.compare("index") == 0) {
        assert_token_size(token.size(), 2);
        std::string value = "";
        for (size_t i = 1; i < token.size(); i++) {
            value += token[i];
        }
        this->setAttribute(key, value);
    }
    token.clear();
  }
}

/*
 * check if necessary fields are present
 */
void Server::validate() {
    if (this->_attrs.find("listen") == this->_attrs.end() ||
            this->_attrs.find("path") == this->_attrs.end() ||
            this->_attrs.find("root") == this->_attrs.end()) {
        throw std::exception();
    }
}

void Server::setAttribute
(std::string const& key, std::string const& value) {
    this->_attrs[key] = value;
}

std::map<std::string, std::string>::const_iterator Server::getAttribute
(std::string const& key) const {
    (void)key;
    return this->_attrs.cbegin();
}

void Server::addLocation(Location location) {
    (void)location;
}

std::vector<Location>::const_iterator Server::getLocations() const {
    return this->_locations.cbegin();
}
