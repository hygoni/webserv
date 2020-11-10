#ifndef SRCS_SERVER_HPP_
#define SRCS_SERVER_HPP_

/* Copyright 2020 hyeyoo */

#include <string>
#include <map>
#include <vector>
#include "Location.hpp"
#include "Context.hpp"

class Server {
 private:
     std::vector<std::string>           _text;
     std::map<std::string, std::string> _attrs;
     std::vector<Location>              _locations;
 public:
     explicit               Server(int fd);
     void                   parseServer();
     void                   validate();
     std::map<std::string, std::string>::const_iterator
                            getAttribute(std::string const& key) const;
     std::vector<Location>::const_iterator
                            getLocations() const;
    void                    addLocation(Location location);
    void                    setAttribute
                            (std::string const& key, std::string const& value);
};

#endif  // SRCS_SERVER_HPP_
