#ifndef SRCS_SERVER_HPP_
#define SRCS_SERVER_HPP_

/* Copyright 2020 hyekim, hyeyoo */

#include "Location.hpp"
#include <string>
#include <map>
#include <vector>
#include <netinet/in.h>
#include "Client.hpp"


class Server {
private:
  std::vector<Client*>                 _clients;
  std::vector<std::string>            _text;
  std::map<std::string, std::string>  _attrs;
  std::vector<Location>               _locations;

  int                                 _listen;
  std::string                         _server_name;
  int                                 _fd;

  /* parsing */
  void                                parse();
  void                                validate();
  void                                addLocation(Location location);

  unsigned short                      ft_htons(unsigned short num);
  unsigned long                       ft_htonl(unsigned long num);
public:
  explicit                            Server(const char *path);
  virtual                             ~Server();

  int                                 initSocket();
  int                                 accept(fd_set *rfds);
   /* getters */
  std::vector<Location> const&        getLocations() const;
  int                                 getListen() const;
  std::string const&                  getServerName() const;
  int                                 getFd() const;
  std::vector<Client*>&               getClients();
};

#endif  // SRCS_SERVER_HPP_
