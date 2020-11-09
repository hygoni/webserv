/* Copyright 2020 hyeyoo */

#include <vector>
#include <utility>
#include "libft.h"
#include "ConfigParser.hpp"
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
}

void Server::parseServer() {
  std::vector<std::string> token;
  bool begin;

  begin = false;
  for (size_t i = 0; i < token.size(); i++) {
    tokenize(this->_text[i], token);
    /* skip empty line */
    if (token.size() <= 0)
      continue;
    /* search for begin first */
    if (!begin) {
      /* only accept "server begin" */
      if (token.size() != 2)
        continue;
      if (token[0].compare("server") == 0) {
        if (token[1].compare("begin") == 0) {
          begin = true;
        } else {
          throw std::exception();
        }
      }
      /* started */
    } else {
      std::string cmd = token[0];
      if (cmd.compare("location") == 0) {
        Location location;
        if (token.size() != 2)
          throw std::exception();
        std::string next = token[1];
        if (next.compare("begin") != 0)
          throw std::exception();
        /* loop until "location begin" */
        do {
          i++;
        } while(cmd.compare("location") != 0);
      } else if (cmd.compare("listen") == 0) {

      } else if (cmd.compare("server_name") == 0) {

      } else if (cmd.compare("server") == 0) {
        
      }
    }

    token.clear();
  }
}

void Server::setAttribute
(std::string const& key, std::string const& value) {

}

std::map<std::string, std::string>::const_iterator Server::getAttribute
(std::string const& key) const {
}

void Server::addLocation(Location location) {
}

std::vector<Location>::const_iterator Server::getLocations() const {
}
