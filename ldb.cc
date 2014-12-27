// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <iostream>
#include <errno.h>

#include "server.h"
#include "proc.h"

int main(int argc, char **argv)
{

    Server server;
    server.Run();
    fprintf(stderr, "init server success\n");

    while (true) {
        ldb_process_events(server);
    }

    return 0;
}            
