#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void ldb_daemon(void) 
{
    int fd;

    if (fork() != 0) {
        exit(0);    /* parent exits */
    }

    setsid();       /* create a new session */

    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) 
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        if (fd > STDERR_FILENO) {
            close(fd);
        }
    }
}

