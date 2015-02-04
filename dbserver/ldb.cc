// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>

#include <dbserver/server.h>
#include <dbserver/signal.h>

Server server;

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

    SigProcess();
    
    if (server.Init(argv[1]) < 0) {
        return -1;
    }

    pid_t pid;
    pid = fork();
    if (pid == 0) { 
        //child
        server.Run();
    } else if (pid < 0) {
        fprintf(stderr, "fork error\n");
        return -1;
    }
    
    //father
    while(1) {
        sleep(100);
    }
    
    return 0;
}            
