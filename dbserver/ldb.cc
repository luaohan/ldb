// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>

#include "server.h"

void Usage(char **argv)
{
    printf("Usage1: %s <server ip> <server port>\n", argv[0]);
    printf("Usage2: %s \n", argv[0]);
    return;
}

int main(int argc, char **argv)
{
    Server server;
    if (argc == 1) {
        if (server.Run("ldb.conf", NULL, 0) < 0) {
            return -1;
        }
    } 
    else if (argc == 3) {
        int port = atoi(argv[2]);
        if (server.Run("ldb.conf", argv[1], port) < 0) {
            printf("Maybe your usage is illegal !\n");
            return -1;
        }
    } 
    else {
        Usage(argv);
    }

    return 0;
}            
