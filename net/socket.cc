// socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "socket.h"

namespace ldb {
namespace net {

void Socket::Notify(int fd, int events, void *arg)
{
    Socket *s = (Socket *)arg;
    s->Process(fd, events);
}

Socket::Socket(ldb::event::Loop *loop): 
    fd_(-1), port_(-1), loop_(loop)
{
}

Socket::Socket(int fd, const std::string &ip, int port, ldb::event::Loop *loop)
    : fd_(fd), ip_(ip), port_(port), loop_(loop)
{
    event_.fd = fd_;
    event_.read = true;
    event_.write = false;
    event_.notify = Socket::Notify;
    event_.arg = (void *)this;

    if (!loop_->Add(event_)) {
        Close();
    }
}

Socket::~Socket()
{
    Close();
}

void Socket::Process(int fd, int events)
{
    assert(handler_ != NULL);
    assert(fd == fd_);

    if (events & EPOLLIN) {
        handler_(owner_, kRead);
    }
    
    if (events & EPOLLOUT) {
        handler_(owner_, kWrite);
    } 
}

void Socket::Close() 
{
    if (fd_ > 0) {

        //note
        if (!loop_->Del(event_)) {
            //LOG(ERROR);
        }

        close(fd_);
        fd_ = -1;
    }
}

int Socket::SetNoblock()
{
    int flags;

    if ((flags = fcntl(fd_, F_GETFL, NULL)) < 0) {        
        return -1;
    }

    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1; 
    }   

    return 0;
}

bool Socket::Connect(const char *ip, int port)
{
    struct sockaddr_in addr;
    int result;
    int on = 1;

    fd_ = socket( AF_INET, SOCK_STREAM, 0 );
    if (fd_ == -1) {
        //LOG(ERROR)
        return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    //block or nonlock?
    result = connect(fd_, (struct sockaddr *) & addr, sizeof(struct sockaddr)); 
    if ( result != 0 ) {
        return false;
    }
    
    ip_ == ip;
    port_ = port;

    event_.fd = fd_;
    event_.read = true;
    event_.write = false;
    event_.notify = Socket::Notify;
    event_.arg = (void *)this;

    if (!loop_->Add(event_)) {
        Close();
        return false;
    }

    return true;
}

void Socket::SetReuseAddr()
{
    int on = 1;
    //return code is ok???
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
}

void Socket::SetLinger()
{
    struct linger optval;
    optval.l_onoff = 1;
    optval.l_linger = 60;
    setsockopt(fd_, SOL_SOCKET, SO_LINGER, (char *)&optval, sizeof(struct linger));
}

void Socket::SetRcvBuf(int size)
{
    setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));
}

void Socket::SetSndBuf(int size)
{
    setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));
}

void Socket::SetNoNagle()
{
    int opt;
    socklen_t optlen;

    optlen = sizeof(opt);  
    if (getsockopt(fd_, IPPROTO_TCP, 1, &opt, &optlen) == -1) {
        return;
    }                                                                  

    if (opt == 1) {
        return;
    }
    opt = 1;
    setsockopt(fd_, IPPROTO_TCP, 1, &opt, sizeof( opt ));

    return;
}

int Socket::Read(char *buffer, int buffer_size)
{
RETRY_READ:
    int bytes = read(fd_, buffer, buffer_size);
    if (bytes == -1) {
        if (errno == EINTR) {
            goto RETRY_READ;
        }

        return -1;
    }

    return bytes;
}

int Socket::Write(char *buffer, int buffer_size)
{
RETRY_WRITE:
    int bytes = write(fd_, buffer, buffer_size);
    if (bytes == -1) {
        if (errno == EINTR) {
            goto RETRY_WRITE;
        }

        return -1;
    }

    return bytes;
}

void Socket::SetHandler(void *owner, Handler handler)
{
    assert(owner != NULL);
    assert(handler != NULL);

    owner_ = owner;
    handler_ = handler;
}

} /*namespace ldb*/
} /*namespace net*/

