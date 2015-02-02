// signal.cc (2015-01-27)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <dbserver/signal.h>
#include <util/log.h>

void SigProcess()
{
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SigChild);

    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = SigtermHandler;
    sigaction(SIGTERM, &act, NULL);
}

static void SigtermHandler(int sig)
{
    quit = true;
    log_info("server exit\n");
}

static void SigChild(int signo)
{
    pid_t pid;
    int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child server exit\n");
        log_info("child server exit\n");
    }

    pid = fork();
    if(pid == 0) { //child
        if (server.Run(arg) < 0) {
            exit(0);
        }
    }

}
