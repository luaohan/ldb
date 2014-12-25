// e_epoll.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _E_EPOLL_H_
#define _E_EPOLL_H_

#if 0
struct EventInfo {
    int fd_;
};

class Epoll {

    public:
        Epoll();
        ~Epoll();

        addEvent(int fd, int event_type);
        delEvent(int fd, int event_type);
        EventInfo *waitEvent(int time_out = -1);

    private:
        int epfd_;

        static const int max_connections_ = 1024;
        struct epoll_event events_[max_connections_];

        struct EventInfo *ready_event[max_connections_];
};
#endif



#endif
