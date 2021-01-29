/* Copyright 2020 hyeyoo */

#include <vector>
#include <utility>
#include <unistd.h>
#include <fcntl.h>
#include "Fd.hpp"
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

Server::~Server() {
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
}

unsigned short     Server::ft_htons(unsigned short num) {
  short result = 1;

  if (((char *)&result)[0] == 0) {
    return num;
  }
	num = num >> 8 | num << 8;
	return (num);
}

unsigned long   Server::ft_htonl(unsigned long num) {
  long result = 1;

  if (((char *)&result)[0] == 0) {
    return num;
  }
  num = (num & 0xff000000) >> 24 | (num & 0x000000ff) << 24 |
		    (num & 0x00ff0000) >> 8  | (num & 0x0000ff00) << 8;
  return num;
}

int  Server::initSocket() {
  struct sockaddr_in server_addr;
  int enable = 1;

  if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    throw "[Server::initSocket] failed whiie gettig socket fd";
  if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    throw "[Server::initSocket] failed while optioning socket";
  // if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
  //   throw std::exception();
  // why ??
  // TODO: htons -> ft_htons, htonl -> ft_htonl
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = ft_htons(_listen);
  server_addr.sin_addr.s_addr = ft_htonl(INADDR_ANY);
  ft_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
  if (bind(_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0 ||
      listen (_fd, 100000) < 0)
    throw std::exception();
  return _fd;
}

int   Server::accept(fd_set *rfds) {
  Client* client = new Client(*this);
  int client_fd = client->getFd();
  _clients.push_back(client);
  Fd::set(client_fd, rfds);
  return client_fd;
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

int Server::getFd() const {
  return this->_fd;
}

std::vector<Client*>&  Server::getClients() {
  return _clients;
}
