// e_epoll.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "e_epoll.h"

Epoll::Epoll()
{
    epfd_ = epoll_create(max_connections_);

    events_ 
        = (struct epoll_event *)malloc( sizeof(struct epoll_event) * max_connections_ );

    memset(fd_in_read_, -1, max_connections_);
    memset(fd_in_write_, -1, max_connections_);
}

Epoll::~Epoll()
{
    if (epfd_ > 0) close(epfd_);
    if (events_ != NULL ) free(events_);
}

int Epoll::addEvent(int fd)
{
    struct epoll_event ee;
    ee.data.fd = fd;
    ee.events = EPOLLIN | EPOLLOUT;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ee);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

int Epoll::delEvent(int fd)
{
    struct epoll_event ee;
    ee.data.fd = fd;
    ee.events = EPOLLIN | EPOLLOUT;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &ee);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

int Epoll::waitEvent(int time_out)
{
    int n = epoll_wait(epfd_, events_, max_connections_, time_out);
    if (n < 0) {
        return -1;
    }

    int nread = 0;
    int nwrite = 0;

    for (int i = 0; i < n; i++)
    {
        if(events_[i].events & EPOLLIN) {

            fd_in_read_[nread++] = events_[i].data.fd;
        } else if (events_[i].events & EPOLLOUT) {

            fd_in_write_[nwrite++] = events_[i].data.fd;
        }
    }

    return 0;
}

int *Epoll::getFdInRead() 
{
    return fd_in_read_;
}

int *Epoll::getFdInWrite() 
{
    return fd_in_write_;
}
