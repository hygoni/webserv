#ifndef SRCS_LOCATION_HPP_
#define SRCS_LOCATION_HPP_

/* Copyright 2020 hyeyoo */

#include <vector>
#include <string>

class Location {
 private:
    std::string                         _path;
    std::string                         _root;
    std::vector<std::string>            _index;
 public:
                                    Location
                                    (std::string const& path, std::string const& root);
    std::string const&              getPath() const;
    std::string const&              getRoot() const;
    std::vector<std::string> const& getIndex() const;
    void                            addIndex(std::string const& index);
    void                            setIndex(std::vector<std::string> index);
};

#endif  // SRCS_LOCATION_HPP_
