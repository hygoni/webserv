#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "Server.hpp"

int     main(void)
{
    int fd = open("./config/server", O_RDONLY);
    if (fd < 0)
        return 1;

    Server server(fd);
    return 0;
}
