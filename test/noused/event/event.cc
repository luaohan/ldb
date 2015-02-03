// e_epoll.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <net/event.h>

Epoll::Epoll()
{
    epfd_ = epoll_create(max_connections_);

    events_ 
        = (struct epoll_event *)malloc( sizeof(struct epoll_event) * max_connections_ );

    assert(epfd_ != -1);
    assert(events_ != NULL);

}

Epoll::~Epoll()
{
    if (epfd_ > 0) {
        close(epfd_);
    }

    if (events_ != NULL ) {
        free(events_);
    }
}


int Epoll::AddReadEvent(const Event &e)
{
    struct epoll_event ee;
    ee.data.fd = e.fd_;
    ee.data.ptr = e.ptr_;

    ee.events = EPOLLIN;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, e.fd_, &ee);
    if (ret == -1) {
        return -1;
    }

    return 0;
}


int Epoll::AddWriteEvent(const Event &e)
{
    struct epoll_event ee;
    ee.data.fd = e.fd_;
    ee.data.ptr = e.ptr_;

    ee.events = EPOLLIN | EPOLLOUT;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, e.fd_, &ee);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

int Epoll::DelWriteEvent(const Event &e)
{
    struct epoll_event ee;
    ee.data.fd = e.fd_;
    ee.data.ptr = e.ptr_;

    ee.events = EPOLLIN;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, e.fd_, &ee);
    if (ret == -1) {
        return -1;
    }

    return 0;
}


int Epoll::DelReadEvent(const Event &e)
{
    struct epoll_event ee;
    ee.data.fd = e.fd_;

    ee.events = EPOLLIN;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, e.fd_, &ee);
    if (ret == -1) {
        return -1;
    }

    return 0;
}


int Epoll::WaitReadEvent(Event *r_events, Event *w_events, int time_out)
{
    int n = epoll_wait(epfd_, events_, max_connections_, time_out);
    if (n < 0) {
        return -1;
    }

    int e_read = 0;
    int e_write = 0;
    for (int i = 0; i < n; i++)
    {
        if(events_[i].events & EPOLLIN) {

            //这里优先处理/上报可读事件，
            //如果写事件与可读事件同时到来，可写事件将被忽略
            r_events[e_read++].ptr_ = events_[i].data.ptr;
            continue; 
        } else if (events_[i].events & EPOLLOUT) {

            w_events[e_write++].ptr_ = events_[i].data.ptr;
        }
    }
    
    r_events[e_read].ptr_ = NULL;
    w_events[e_write].ptr_ = NULL;

    return n;
}

