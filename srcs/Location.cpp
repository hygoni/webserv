/* Copyright 2020 hyeyoo */

#include "Location.hpp"
#include "Server.hpp"
#include "tokenize.hpp"
#include "libft.h"
#include <utility>
#include <fcntl.h>
#include <unistd.h>

Location::Location(const char *path) {
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

void Location::validate() {
    if (this->_attrs.find("path") == this->_attrs.end() ||
            this->_attrs.find("root") == this->_attrs.end()) {
        throw std::exception();
    }
    this->_path = this->_attrs["path"];
    this->_root = this->_attrs["root"];
    
    if (this->_attrs.find("cgi_path") != this->_attrs.end())
      this->_cgi_path = this->_attrs["cgi_path"];
    if (this->_attrs.find("directory_listing") != this->_attrs.end()) {
      if (this->_attrs["directory_listing"].compare("on") == 0)
        this->_directory_listing = true;
      else
        this->_directory_listing = false;
    }
    if (this->_attrs.find("default_error_page") != this->_attrs.end())
      this->_default_error_page = this->_attrs["default_error_page"];
}

void Location::parse() {
  std::vector<std::string> token;

  /* init default values */

  /* save into map */
  for (size_t i = 0; i < this->_text.size(); i++) {
    tokenize(this->_text[i], token);
    /* skip empty line */
    if (token.size() <= 0)
      continue;
    std::string key = token[0];
    if (key.compare("path") == 0 ||
            key.compare("root") == 0 ||
            key.compare("cgi_path") == 0 ||
            key.compare("default_error_page") == 0 ||
            key.compare("directory_listing") == 0) {
        assert_token_size(token.size(), 2);
        std::string value = token[1];
        this->_attrs[key] = value;
    } else if (key.compare("index") == 0) {
        assert_token_size(token.size(), 2);
        for (size_t i = 1; i < token.size(); i++) {
            this->_index.push_back(token[i]);
        }
    } else if (key.compare("cgi_extension") == 0) {
        assert_token_size(token.size(), 2);
        for (size_t i = 1; i < token.size(); i++) {
            this->_cgi_extension.push_back(token[i]);
        }
    } else if (key.compare("allowed_method") == 0) {
        assert_token_size(token.size(), 2);
        for (size_t i = 1; i < token.size(); i++) {
            this->_allowed_method.push_back(token[i]);
        }
    }
    token.clear();
  }

  /* validation */
  validate(); 
}

std::string const& Location::getPath() const {
    return this->_path;
}

std::string const& Location::getRoot() const {
    return this->_root;
}

std::vector<std::string> const& Location::getIndex() const {
    return this->_index;
}

void Location::addIndex(std::string const& index) {
    this->_index.push_back(index);
}

void Location::setIndex(std::vector<std::string> index) {
    this->_index = index;
}
