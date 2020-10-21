/* Copyright 2020 hyeyoo */

#include <utility>
#include "Server.hpp"

void Server::setAttribute
(std::string const& key, std::string const& value) {
    this->_attrs.insert(std::make_pair(key, value));
}

std::map<std::string, std::string>::const_iterator Server::getAttribute
(std::string const& key) const {
    return this->_attrs.find(key);
}

void Server::addLocation(Location location) {
    this->_locations.push_back(location);
}

std::vector<Location>::const_iterator Server::getLocations() const {
    return this->_locations.begin();
}
