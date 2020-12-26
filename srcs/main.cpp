#include "Config.hpp"
#include "ServerManager.hpp"
#include <iostream>
int main(void) {
  Config::createInstance("./config/config");
  Config* config = Config::getInstance();
  try {
    ServerManager server_manager(*config);
    server_manager.run();
  } catch (const char* msg) {
    std::cout << msg << std::endl;
  }
  return (0);
}

