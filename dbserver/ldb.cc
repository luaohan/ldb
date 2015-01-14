// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)


#include "server.h"

int main(int argc, char **argv)
{
    Server server;
    if (server.Run("ldb.conf") < 0) {
        return -1;
    }


    return 0;
}            
