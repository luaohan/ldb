// e_epoll.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _E_EPOLL_H_
#define _E_EPOLL_H_

#include <sys/epoll.h>

namespace ldb {
namespace event {

class Event {

    public:
        Event();
        ~Event();

        int AddEvent(int fd, int mark);
        int DelEvent(int fd, int mark);

        //ok: return the num of the events 
        //error: return -1
        int WaitEvent(int *fired_fd, int time_out = -1);

        static const int E_EPOLLIN = EPOLLIN;
        static const int E_EPOLLOUT = EPOLLOUT;


    private:
        int epfd_;

        static const int max_connections_ = 1024;
        struct epoll_event *events_;
};

} /*namespace ldb*/
} /*namespace event*/

#endif
