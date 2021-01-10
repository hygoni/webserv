#include "Config.hpp"
#include "ServerManager.hpp"
#include "debug.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <execinfo.h>

void
handler()
{
    void *trace_elems[20];
    int trace_elem_count(backtrace( trace_elems, 20 ));
    char **stack_syms(backtrace_symbols( trace_elems, trace_elem_count ));
    for ( int i = 0 ; i < trace_elem_count ; ++i )
    {
        std::cout << stack_syms[i] << "\n";
    }
    free( stack_syms );
    exit(1);
}

int main(void) {
  Config::createInstance("./config/config");
  Config* config = Config::getInstance();
  std::set_terminate(handler);
  try {
    ServerManager server_manager(*config);
    server_manager.run();
  } catch (const char* msg) {
    std::cout << msg << std::endl;
    std::cout << strerror(errno) << std::endl;
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }
  return (0);
}

