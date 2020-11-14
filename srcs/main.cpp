#include <iostream>
#include "Server.hpp"
#include "Config.hpp"

int     main(void)
{
    Config::createInstance("./config/config");
    Config* config = Config::getInstance();

    (void)config;

    return 0;
}
