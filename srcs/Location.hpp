#ifndef SRCS_LOCATION_HPP_
#define SRCS_LOCATION_HPP_

/* Copyright 2020 hyeyoo */

#include <map>
#include <string>

class Location {
 private:
    std::string                         _path;
    std::map<std::string, std::string>  _attrs;
 public:
    std::map<std::string, std::string>::const_iterator
                            getAttribute(std::string const& key) const;
    std::string const&      getPath() const;
    void                    setPath(std::string const& path);
    void                    setAttribute
                            (std::string const& key, std::string const& value);
};

#endif  // SRCS_LOCATION_HPP_