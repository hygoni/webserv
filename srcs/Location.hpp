#ifndef SRCS_LOCATION_HPP_
#define SRCS_LOCATION_HPP_

/* Copyright 2020 hyeyoo */

#include <vector>
#include <string>
#include <map>

class Location {
 private:
   std::vector<std::string>             _text;
    std::map<std::string, std::string>  _attrs;
    std::string                         _path;
    std::string                         _root;
    std::vector<std::string>            _index;
    std::vector<std::string>            _allowed_method;
    std::string                         _cgi_path;
    std::vector<std::string>            _cgi_extension;
    std::string                         _authorization;
    bool                                _directory_listing;
    std::string                         _default_error_page;
    bool                                _cgi;

 public:
                                    Location(const char *path);
    void                            parse();
    void                            validate();
    void                            addIndex(std::string const& index);
    void                            setIndex(std::vector<std::string> index);
    /* getters */
    std::string const&              getPath() const;
    std::string const&              getRoot() const;
    std::vector<std::string> const& getIndex() const;
    std::vector<std::string> const& getAllowedMethod() const;
    std::string const&              getCgiPath() const;
    std::vector<std::string> const& getCgiExtension() const;
    bool                            getDirectoryListing() const;
    std::string const&              getDefaultErrorPage() const;
    std::string const&              getAuthorization() const;
    bool                            isCgi() const;
};

#endif  // SRCS_LOCATION_HPP_
