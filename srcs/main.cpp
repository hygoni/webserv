#include "Config.hpp"
#include "ServerManager.hpp"
#include "debug.hpp"
#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <execinfo.h>

int main(void) {
  Config::createInstance("./config/config");
  Config* config = Config::getInstance();
  try {
    ServerManager server_manager(*config);
    server_manager.run();
  } catch (const char* msg) {
    std::cout << msg << std::endl;
    std::cout << strerror(errno) << std::endl;
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    std::cout << strerror(errno) << std::endl;
  }
  return (0);
}

