#ifndef SRCS_SERVER_HPP_
#define SRCS_SERVER_HPP_

/* Copyright 2020 hyeyoo */

#include <string>
#include <map>
#include <vector>
#include "Location.hpp"

class Server {
 private:
     std::vector<std::string>           _text;
     std::map<std::string, std::string> _attrs;
     std::vector<Location>              _locations;

     int                                _listen;
     std::string                        _server_name;
     int                                _client_body_size_limit;
     int                                _client_header_size_limit;

 public:
     explicit               Server(const char *path);
     /* parsing */
     void                   parse();
     void                   validate();
     void                   addLocation(Location location);
     /* getters */
     std::vector<Location> const&
                            getLocations() const;
     int                    getListen() const;
     std::string const&     getServerName() const;
};

#endif  // SRCS_SERVER_HPP_
