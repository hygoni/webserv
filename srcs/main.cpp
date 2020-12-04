#include "Config.hpp"
#include "ServerManager.hpp"
#include <iostream>
int main(void) {
  Config::createInstance("./config/config");
  Config* config = Config::getInstance();

  ServerManager server_manager(*config);
  server_manager.run();
  return (0);
}

