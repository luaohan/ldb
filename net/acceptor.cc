// acceptor.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>

#include <util/log.h>
#include "socket.h"
#include "acceptor.h"

namespace ldb {
namespace net {

void Acceptor::Notify(int fd, int events, void *arg)
{
    Acceptor *a = (Acceptor *)arg;
    a->Process(fd, events);
}

Acceptor::Acceptor(ldb::event::Loop *loop): 
    fd_(-1), port_(-1), backlog_(-1), loop_(loop)
{
}

Acceptor::~Acceptor()
{
    Close();
}

void Acceptor::Process(int fd, int events)
{
    if (events & EPOLLIN) {
        //process read event
        //call Accept();
    }
    
    if (events & EPOLLOUT) {
        //process write event
        //nothing to do for listen socket
    }
}

int Acceptor::fd() const
{
    return fd_;
}

int Acceptor::SetNonBlock()
{
    assert(fd_ != -1);

    int flags;

    if ((flags = fcntl(fd_, F_GETFL, NULL)) < 0) {
        //LOG(ERROR)
        return -1;
    }

    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
        //LOG(ERROR)
        return -1; 
    }   

    return 0;
}

int Acceptor::Listen(const std::string &ip, int port, int backlog)
{
    assert(backlog > 0);
    assert(port > 1000 && port < 65535);

    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ == -1) {
        //LOG(ERROR)
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (ip == "0.0.0.0") {
        addr.sin_addr.s_addr = INADDR_ANY; //htonl(INADDR_ANY);
    } else {
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }
    addr.sin_port = htons(port);

    int rc = bind(fd_, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (rc == -1 ) {
        //LOG(ERROR)
        close(fd_);
        fd_ = -1;
        return -1;
    }
    
    if (listen(fd_, backlog_) == -1 ) {
        //LOG(ERROR)
        Close();
        return -1;
    }

    backlog_ = backlog;
    ip_ = ip;
    port_ = port;

    event_.fd = fd_;
    event_.read = true;
    event_.write = false;
    event_.notify = Acceptor::Notify;
    event_.arg = (void *)this;

    return 0;
}


void Acceptor::Close()
{
    if (fd_ > 0)  {
        //note
        if (!loop_->Del(event_)) {
            //LOG(ERROR)
        }

        close(fd_);
        fd_ = -1;
    }
}

Socket *Acceptor::Accept()
{
    int fd = -1;
    struct sockaddr_in remote;
    int len = 0;

ReAccept:
    len = sizeof( struct sockaddr_in );

    fd = accept(fd_, (struct sockaddr *)&remote, (socklen_t *)&len);
    if (fd == -1) {
        int err = errno;
        if ( err == EINTR) {
            goto ReAccept;
        }
//#ifdef  EPROTO
//        if (err == EPROTO || err == ECONNABORTED) {
//            goto ReAccept;
//        }
//#else
//        if (err == ECONNABORTED ) {
//            goto ReAccept;
//        }
//#endif
        //LOG(ERROR)
        return NULL;  
    }

    Socket *socket = new Socket(fd, inet_ntoa(remote.sin_addr), ntohs(remote.sin_port), loop_);
    if (socket == NULL) {
        //LOG(ERROR)
        abort();
        return NULL;
    }

    return socket;
}

void Acceptor::SetReuseAddr()
{
    int on = 1;                          
    int rc = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    //if (rc == -1) {
    //    //LOG(ERROr)
    //}
}

} /*namespace ldb*/
} /*namespace net*/

