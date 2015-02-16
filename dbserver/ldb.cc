// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>

#include <dbserver/server.h>
#include <dbserver/signal.h>
#include <util/log.h>
#include <util/config.h>
#include <util/daemon.h>

pid_t father_id;
Server *server;

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

    father_id = getpid();
   
    std::string file = argv[1];
    Config config;
    int ret = config.LoadConfig(file);
    if (ret != 0) {
        fprintf(stderr, "configfile is wrong.\n");
        return -1;
    }
    
    if (config.daemon_) {
        Daemon();
    }

    pid_t pid;
    pid = fork();
    if (pid == 0) { 
        //child
        
        ChildSigProcess();
    
        log_info("child id: %d\n", getpid());
        server = new Server(config);
        if (server->Run() < 0) {
            kill(father_id, SIGINT);
            sleep(1); //确保父进程已近退出
            exit(1);
        }

    } else if (pid < 0) {
        fprintf(stderr, "fork error\n");
        return -1;
    }

    //father
    FatherSigProcess();
    while(1) {
        sleep(100);
    }

    return 0;
}            
