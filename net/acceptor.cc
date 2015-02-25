// acceptor.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "acceptor.h"
#include "async_socket.h"

void Acceptor::Notify(int fd, short what, void *data)
{
    if (what & EV_READ) {
        Acceptor *acceptor = (Acceptor *)data;
        acceptor->handler_(acceptor->data_);
    }
}

bool Acceptor::SetNonBlock()
{
    int flags;
    if ((flags = fcntl(fd_, F_GETFL, NULL)) < 0) {
        return false;
    }

    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
        return false;
    }

    return true;
}

int Acceptor::Listen(const std::string &ip, int port)
{
    assert(!ip.empty());

    int rc = 0;
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ == -1) {
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (ip == "0.0.0.0") {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }
    addr.sin_port = htons(port);

    if (!SetNonBlock()) {
        goto ErrReturn;
    }

    if (!SetReuseAddr()) {
        goto ErrReturn;
    }

    rc = bind(fd_, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (rc == -1 ) {
        goto ErrReturn;
    }

    if (listen(fd_, 500) == -1 ) {
        goto ErrReturn;
    }

    event_ = event_new(base_, fd_, 
                EV_READ | EV_PERSIST, Notify, this);
    assert(event_ != NULL);
    event_add(event_, NULL);

    ip_ = ip;
    port_ = port;

    return 0;

ErrReturn:
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }

    return -1;
}

void Acceptor::Close()
{
    if ( fd_ > 0 ) {
        if (event_ != NULL) {
            event_free(event_);
            event_ = NULL;
        }

        close(fd_);
        fd_ = -1;
    }
}

AsyncSocket *Acceptor::Accept()
{
    struct sockaddr_in remote;

ReAccept:
    int len = sizeof( struct sockaddr_in );
    int fd = accept(fd_, (struct sockaddr *)&remote, (socklen_t *)&len);
    if (fd == -1) {
        if (errno == EINTR) {
            goto ReAccept;
        }

        return NULL;
    }

    AsyncSocket *s = new AsyncSocket(base_, fd, 
            inet_ntoa(remote.sin_addr), 
            ntohs(remote.sin_port));
    assert(s != NULL);

    s->SetNoNagle();
    s->SetNonBlock();

    return s;
}

bool Acceptor::SetReuseAddr()
{
    int on = 1;
    int rc = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc == -1) {
        return false;
    }

    return true;
}
