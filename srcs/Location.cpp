/* Copyright 2020 hyeyoo */

#include "Location.hpp"
#include <utility>

Location::Location(Context& ctx) : _ctx(ctx) {}

bool Location::parseLocation(Context& ctx) {

}

void Location::setAttribute
(std::string const& key, std::string const& value) {
    this->_attrs.insert(std::make_pair(key, value));
}

void Location::setPath(std::string const& path) {
    this->_path = path;
}

std::map<std::string, std::string>::const_iterator Location::getAttribute
(std::string const& key) const {
    return this->_attrs.find(key);
}

std::string const& Location::getPath() const {
    return this->_path;
}
