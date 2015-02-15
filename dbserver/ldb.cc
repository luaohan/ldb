// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <assert.h>

#include <dbserver/server.h>
#include <dbserver/signal.h>
#include <util/log.h>

Server *server;
pid_t father_id;

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
    
    father_id = getpid();

    server = new Server;
    assert(server != NULL);

    if (server->Init(argv[1]) < 0) {
        return -1;
    }

    pid_t pid;
    pid = fork();
    if (pid == 0) { 
        //child
        log_info("child id: %d\n", getpid());
        server->Run();
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
