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
    Server server;
    server.Run("./ldb.conf");
#if 0
    if ( server.config_.info_log_file_ ) {
        int fd_info = open(server.config_.info_log_file_, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        std::string info_log_path = server.config_.info_log_file_;
        info_log = new Log(fd_info, info_log_path, false);
    }

    if (server.config_.error_log_file_ ) {
        int fd_error = open(server.config_.error_log_file_, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        std::string error_log_path = server.config_.error_log_file_;
        error_log = new Log(fd_error, error_log_path, false);
    }
#endif 
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
