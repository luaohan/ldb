// e_epoll.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _E_EPOLL_H_
#define _E_EPOLL_H_

#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <net/socket.h>

struct TimeEvent {
    TimeEvent(Socket *s, unsigned long long time, std::string ip, int port):
        s_(s), time_(time), ip_(ip), port_(port){}
    
    Socket *s_;
    unsigned long long time_; //微妙，如果当前的时间大于这个时间，就执行时间事件
    std::string ip_;
    int port_;
};

struct Event {
    Event():fd_(-1), ptr_(NULL){}

    int fd_;
    void *ptr_;
};

class Epoll {

    public:
        Epoll();
        ~Epoll();
        
        int AddReadEvent(const Event &e);
        int AddWriteEvent(const Event &e);
        int DelWriteEvent(const Event &e);
        int DelReadEvent(const Event &e);

        //ok: return the num of the events 
        //error: return -1
        //int WaitReadEvent(int *fired_fd, int time_out = -1);
        int WaitReadEvent(Event *r_events, Event *w_events, int time_out = -1);

    private:
        int epfd_;

        static const int max_connections_ = 1024;
        struct epoll_event *events_;
};


#endif
