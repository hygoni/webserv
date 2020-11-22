#ifndef SRCS_SERVER_HPP_
#define SRCS_SERVER_HPP_

/* Copyright 2020 hyekim, hyeyoo */

#include "Location.hpp"
#include <string>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Client.hpp"


class Server {
private:
  std::vector<Client>                 _clients;
  std::vector<std::string>            _text;
  std::map<std::string, std::string>  _attrs;
  std::vector<Location>               _locations;

  int                                 _listen;
  std::string                         _server_name;
  int                                 _client_body_size_limit;
  int                                 _client_header_size_limit;

  int                                 _fd;

  short                               ft_htons(short num);
  long                                ft_htonl(long num);
public:
  explicit                            Server(const char *path);
  /* parsing */
  void                                parse();
  void                                validate();
  void                                addLocation(Location location);

  int                                 initSocket();
  int                                 accept();
   /* getters */
  std::vector<Location> const&        getLocations() const;
  int                                 getListen() const;
  std::string const&                  getServerName() const;
  int                                 getClientBodySizeLimit() const;
  int                                 getClientHeaderSizeLimit() const;
  int                                 getFd() const;
};

#endif  // SRCS_SERVER_HPP_
