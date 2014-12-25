// e_epoll.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _E_EPOLL_H_
#define _E_EPOLL_H_

#include <sys/epoll.h>

class Epoll {

    public:
        Epoll();
        ~Epoll();

        int addEvent(int fd);
        int delEvent(int fd);

        //ok: return 0
        //error: return -1
        int waitEvent(int time_out = -1);

        int *getFdInRead() ;
        int *getFdInWrite() ;

    private:
        int epfd_;

        static const int max_connections_ = 1024;
        struct epoll_event *events_;

        int fd_in_read_[max_connections_];
        int fd_in_write_[max_connections_];

};



#endif
