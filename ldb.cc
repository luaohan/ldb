// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "server.h"
#include "proc.h"
#include "log.h"

extern Log *info_log;
extern Log *error_log;

int main(int argc, char **argv)
{
    if ( 1 ) {
        int fd_info = open("./info_log.txt", O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        std::string info_log_path = "./info_log.txt";
        info_log = new Log(fd_info, info_log_path, false);
    }

    if ( 1 ) {
        int fd_error= open("./error_log.txt", O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        std::string error_log_path = "./error_log.txt";
        error_log = new Log(fd_error, error_log_path, false);
    }

    Server server;
    server.Run();
    fprintf(stderr, "init server success\n");

    while (true) {
        ldb_process_events(server);
    }

    if (info_log != NULL) {
        delete info_log;
    }

    if (error_log != NULL) {
        delete error_log;
    }

    return 0;
}            
