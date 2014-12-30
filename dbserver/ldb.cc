// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <util/log.h>
#include "server.h"

using namespace ldb::dbserver;

int main(int argc, char **argv)
{
    Server server;
    server.Run("./ldb.conf");
    
    //fprintf(stderr, "init server success\n");

    //while (true) {
    //    process_events(server);
    //}

    return 0;
}            
