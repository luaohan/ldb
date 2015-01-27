// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>

#include <dbserver/signal.h>
#include "server.h"

Server server;
char *arg;

void Usage(char **argv)
{
    printf("Usage: %s <configfile>\n", argv[0]);
    return;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        Usage(argv);
        return -1;
    }

    arg = argv[1];
    SigProcess();

    pid_t pid;
    pid = fork();
    if (pid == 0) {
        if (server.Run(argv[1]) < 0) {
            exit(0);
        }
    }

    //father
    while(1) {
        sleep(10);
    }
    
    return 0;
}            
