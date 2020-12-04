#ifndef SRCS_SERVER_MANAGER_HPP_
# define SRCS_SERVER_MANAGER_HPP_
# include "Server.hpp"
# include "Config.hpp"
# include "libft.h"
# include <sys/select.h>
class ServerManager {
private:
  std::vector<Server>  _servers;
  ServerManager();
public:
  ServerManager(Config const & config);
  ~ServerManager();

  void  run();
};
#endif
