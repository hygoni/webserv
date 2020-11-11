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

void validateLocation(std::map<std::string, std::string> location) {
    if (location.find("path") == location.end() ||
            location.find("root") == location.end()) {
        throw std::exception();
    }
}

void Server::parseServer() {
  std::vector<std::string> token;
  std::map<std::string, std::string> location_map;
  std::vector<std::string> index;

  for (size_t i = 0; i < this->_text.size(); i++) {
    tokenize(this->_text[i], token);
    /* skip empty line */
    if (token.size() <= 0)
      continue;
    std::string key = token[0];
    if (key.compare("listen") == 0 ||
            key.compare("server_name") == 0) {
        assert_token_size(token.size(), 2);
        std::string value = token[1];
        this->_attrs[key] = value;
    } else if (key.compare("path") == 0 ||
            key.compare("root == 0")) {
            assert_token_size(token.size(), 2);
            std::string value = token[1];
            location_map[key] = value;

    } else if (key.compare("index") == 0) {
        assert_token_size(token.size(), 2);
        for (size_t i = 1; i < token.size(); i++) {
            index.push_back(token[i]);
        }
    }
    token.clear();
  }

  /* validation */
  validate();
  this->_listen = stoi(this->_attrs["listen"]); /* caution: parse error */
  this->_server_name = this->_attrs["server_name"];
  validateLocation(location_map);
  
  Location location(location_map["path"], location_map["path"]);
  location.setIndex(index);
  this->addLocation(location);
}

/*
 * check if necessary fields are present
 */
void Server::validate() {
    if (this->_attrs.find("listen") == this->_attrs.end() ||
            this->_attrs.find("server_name") == this->_attrs.end()) {
        throw std::exception();
    }
}

void Server::addLocation(Location location) {
    this->_locations.push_back(location);
}

std::vector<Location> const& Server::getLocations() const {
    return this->_locations;
}

int Server::getListen() const {
    return this->_listen;
}

std::string const& Server::getServerName() const {
    return this->_server_name;
}
