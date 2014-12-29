// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "server.h"
#include "proc.h"
#include "log.h"

extern Log *info_log;
extern Log *error_log;

int main(int argc, char **argv)
{
    Server server;
    server.Run("./ldb.conf");
    
    fprintf(stderr, "init server success\n");

    while (true) {
        ldb_process_events(server);
    }

    if (info_log != NULL) {
        delete info_log;
    }

    if (error_log != NULL) {
        delete error_log;
    }

    return 0;
}            
