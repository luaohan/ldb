// e_epoll.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "event.h"

namespace ldb {
namespace event {

Loop::Loop() : quit_(false)
{
    epfd_ = epoll_create(event_size);
    assert(epfd_ != -1);
}

Loop::~Loop()
{
    if (epfd_ != -1) {
        close(epfd_);
    }
}

bool Loop::Add(const Event &e)
{
    assert(e.fd >= 0);
    assert(e.read || e.write);
    //assert(e->arg != NULL);

    struct epoll_event ee;
    ee.data.fd = e.fd;
    int events = 0;
    if (e.read) {
        events |= EPOLLIN;
    }
    if (e.write) {
        events |= EPOLLOUT;
    }
    ee.events = events;
    ee.data.ptr = (void *)(const_cast<Event *>(&e));

    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, e.fd, &ee);
    if (ret == -1) {
        return false;
    }

    return true;
}

bool Loop::Del(const Event &e)
{
    struct epoll_event ee;
    ee.data.fd = e.fd;
    ee.events = EPOLLIN | EPOLLOUT;

    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, e.fd, &ee);
    if (ret == -1) {
        return false;
    }

    return true;
}

bool Loop::Run()
{
    quit_ = false;

    struct epoll_event *events
        = (struct epoll_event *)malloc( sizeof(struct epoll_event) * event_size);
    if (events == NULL) {
        assert(events != NULL);
        return false;
    }

    do {
        int n = epoll_wait(epfd_, events, event_size, -1);
        if (n <= 0) {
            break;
        }

        for (int i = 0; i < n; i++) {
            Event *e = (Event *)events[i].data.ptr;
            //assert(e->fd == events[i].data.fd);
            e->notify(e->fd, events[i].events, e->arg);
        }
    } while (!quit_);

    free(events);

    return true;
}

void Loop::Stop()
{
    quit_ = true;
    //pipe is a better method
    if (epfd_ != -1) {
        close(epfd_);
    }
}

} /*namespace ldb*/
} /*namespace event*/

