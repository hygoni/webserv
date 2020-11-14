#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "Server.hpp"

int     main(void)
{
    Server server("./config/server");
    return 0;
}
