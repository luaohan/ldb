// e_epoll.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _E_EPOLL_H_
#define _E_EPOLL_H_

#include <sys/epoll.h>

namespace ldb {
namespace event {

typedef void (*Notify)(int fd, int events, void *arg);

struct Event {
    Event() : fd(-1), read(false), write(false), notify(NULL), arg(NULL) {}
    //explicit Event(int fd, bool read, bool write, Notify notify, void *arg)
    //    : fd(fd), read(read), write(write), notify(notify), arg(arg) {}

    int fd;
    bool read;
    bool write;
    Notify notify;
    void *arg;
};

class Loop {
public:
    Loop();
    ~Loop();

    enum {
        event_size = 1024
    };

    bool Add(const Event &e);
    bool Del(const Event &e);
    bool Run();
    void Stop();

private:
    int epfd_;
    bool quit_;
};

} /*namespace ldb*/
} /*namespace event*/

#endif
