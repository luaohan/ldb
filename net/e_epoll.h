// e_epoll.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _E_EPOLL_H_
#define _E_EPOLL_H_

#include <sys/epoll.h>

class Epoll {

    public:
        Epoll();
        ~Epoll();

        int addReadEvent(int fd);
        int delReadEvent(int fd);

        //ok: return the num of the events 
        //error: return -1
        int waitReadEvent(int *fired_fd, int time_out = -1);

    private:
        int epfd_;

        static const int max_connections_ = 1024;
        struct epoll_event *events_;
};


#endif
