// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "server.h"
#include "proc.h"
#include "../util/log.h"

int main(int argc, char **argv)
{
    Server server;
    if (server.Run("ldb.conf") < 0) {
        return -1;
    }

    fprintf(stderr, "init server success\n");

    while (!quit) {
        process_events(server);
    }

    if (log != NULL) {
        delete log;
    }

    return 0;
}            
