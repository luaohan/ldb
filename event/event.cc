// e_epoll.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "event.h"

namespace ldb {
namespace event {

Event::Event()
{
    epfd_ = epoll_create(max_connections_);

    events_ 
        = (struct epoll_event *)malloc( sizeof(struct epoll_event) * max_connections_ );

}

Event::~Event()
{
    if (epfd_ > 0) close(epfd_);
    if (events_ != NULL ) free(events_);
}

int Event::AddEvent(int fd, int mark)
{
    struct epoll_event ee;
    ee.data.fd = fd;
    ee.events = mark;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ee);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

int Event::DelEvent(int fd, int mark)
{
    struct epoll_event ee;
    ee.data.fd = fd;
    ee.events = mark;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &ee);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

int Event::WaitEvent(int *fired_fd, int time_out)
{
    int n = epoll_wait(epfd_, events_, max_connections_, time_out);
    if (n < 0) {
        return -1;
    }

    for (int i = 0; i < n; i++)
    {
        if(events_[i].events & EPOLLIN || events_[i].events & EPOLLOUT) {

            fired_fd[i] = events_[i].data.fd;
        } 
    }

    return n;
}

} /*namespace ldb*/
} /*namespace event*/

