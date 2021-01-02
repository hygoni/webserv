#include "Config.hpp"
#include "ServerManager.hpp"
#include "debug.hpp"
#include <iostream>
int main(void) {
  Config::createInstance("./config/config");
  Config* config = Config::getInstance();
  try {
    ServerManager server_manager(*config);
    server_manager.run();
  } catch (const char* msg) {
    log("%s\n", msg);
  }
  return (0);
}

