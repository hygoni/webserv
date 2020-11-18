/* Copyright 2020 hyeyoo */

#include <vector>
#include <utility>
#include <unistd.h>
#include <fcntl.h>
#include "libft.h"
#include "tokenize.hpp"
#include "Server.hpp"

/*
 * generate text
 */
Server::Server(const char *path) {
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

void Server::parse() {
  std::vector<std::string> token;
  std::map<std::string, std::string> location_map;
  std::vector<std::string> index;

  /* init default values */
  this->_client_body_size_limit = -1;
  this->_client_header_size_limit = -1;

  /* save into map */
  for (size_t i = 0; i < this->_text.size(); i++) {
    tokenize(this->_text[i], token);
    /* skip empty line */
    if (token.size() <= 0)
      continue;
    std::string key = token[0];
    if (key.compare("listen") == 0 ||
            key.compare("server_name") == 0 ||
            key.compare("client_body_size_limit") == 0 ||
            key.compare("client_header_size_limit") == 0) {
        assert_token_size(token.size(), 2);
        std::string value = token[1];
        this->_attrs[key] = value;
    } else if (key.compare("location") == 0) {
      assert_token_size(token.size(), 2);
      Location location(token[1].c_str());
      this->_locations.push_back(location);
    }
    token.clear();
  }

  /* validation */
  validate(); 
}

/*
 * check if necessary fields are present
 */
void Server::validate() {
    if (this->_attrs.find("listen") == this->_attrs.end() ||
            this->_attrs.find("server_name") == this->_attrs.end()) {
        throw std::exception();
    }
    this->_listen = stoi(this->_attrs["listen"]); /* caution: parse error */
    this->_server_name = this->_attrs["server_name"];
    if (this->_attrs.find("client_body_size_limit") != this->_attrs.end()) {
      this->_client_body_size_limit = 
        std::stoi(this->_attrs["client_body_size_limit"]);
    }
    if (this->_attrs.find("client_header_size_limit") != this->_attrs.end()) {
      this->_client_header_size_limit = 
        std::stoi(this->_attrs["client_header_size_limit"]);
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

int Server::getClientBodySizeLimit() const {
    return this->_client_body_size_limit;
}

int Server::getClientHeaderSizeLimit() const {
    return this->_client_header_size_limit;
}
