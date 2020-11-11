/* Copyright 2020 hyeyoo */

#include "Location.hpp"
#include <utility>

Location::Location
(std::string const& path, std::string const& root) {
    this->_path = path;
    this->_root = root;
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

