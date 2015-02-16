// signal.cc (2015-01-27)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <event2/event.h>

#include <dbserver/signal.h>
#include <util/log.h>

void ChildSigProcess()
{
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SigTermHandler);

    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = SigTermHandler;
    sigaction(SIGTERM, &act, NULL);
}

void FatherSigProcess()
{
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SigChildHandler);
    signal(SIGINT, SigTermHandler);

    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = SigTermHandler;
    sigaction(SIGTERM, &act, NULL);
}

static void SigTermHandler(int sig)
{
    log_info("server exit\n");
    
    if (getpid() != father_id) {
        //child
        server->Stop();
        delete server;
        delete log;
        sleep(1);  //让父进程先退
        exit(0);

    } else {
        //father
        exit(0);
    }
}

static void SigChildHandler(int signo)
{
    pid_t pid;
    int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        log_info("child server: %d exit \n", pid);
    }

    pid = fork();
    if(pid == 0) { //child
        log_info("new child: %d server start\n", getpid());
        ChildSigProcess();
        server = new Server(config);
        server->Run();
    }
}
