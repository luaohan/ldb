// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>

#include "server.h"

void Usage(char **argv)
{
    printf("Usage: %s <configfile>\n", argv[0]);
    return;
}

int main(int argc, char **argv)
{
    Server server;
    if (argc == 2) {
        if (server.Run(argv[1]) < 0) {
            return -1;
        }
    } else {
        Usage(argv);
    }

    return 0;
}            
