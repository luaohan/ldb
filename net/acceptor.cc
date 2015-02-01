// acceptor.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "acceptor.h"
#include "socket.h"

Acceptor::Acceptor():
    fd_(-1), port_(-1), backlog_(-1), is_noblocked_(false), event_(NULL)
{
    ip_[0] = '\0';

    fd_ = socket( AF_INET, SOCK_STREAM, 0 );

    assert(fd_ != -1);
}

Acceptor::~Acceptor()
{
    if (event_ != NULL) {
        event_free(event_);
    }
    
    if ( fd_ > 0 ) {
        close(fd_);
        fd_ = -1;
    }
}

int Acceptor::fd() const
{
    return fd_;
}

int Acceptor::backlog() const
{
    return backlog_;
}

int Acceptor::port() const
{
    return port_;
}

int Acceptor::SetNonBlock()
{
    int flags;
    if ((flags = fcntl(fd_, F_GETFL, NULL)) < 0) {
        return -1;
    }
   
    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }
   
    is_noblocked_ = true;
   
    return 0;
}

bool Acceptor::IsNoblocked() const
{
    return is_noblocked_;
}

int Acceptor::Listen(const char *ip, int port, int backlog)
{
    assert(backlog > 0);
    assert(ip != NULL);
    backlog_ = backlog;
    struct sockaddr_in addr;
  
    addr.sin_family = AF_INET;
    if (strcmp(ip, "0.0.0.0") == 0) {
        addr.sin_addr.s_addr = INADDR_ANY; //htonl(INADDR_ANY);
    } else {
        addr.sin_addr.s_addr = inet_addr( ip );
    }

    addr.sin_port = htons( port );
    if (bind(fd_, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1 ) {
        close( fd_);
        fd_ = -1;
        return -1;
    }
   
    if (listen(fd_, backlog_) == -1 ) {
        close(fd_);
        fd_ = -1;
        return -1;
    }
   
    strcpy(ip_, ip);
    port_ = port;
  
    return 0;
}

void Acceptor::Close()
{
    if (fd_ > 0) {
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
        if ( err == EINTR ) {
            goto ReAccept;
        }
        
        return NULL;
    }
   
    Socket *socket = new Socket;
    if (socket == NULL) {
        return NULL;
    }

    socket->set_fd(fd);
    socket->set_port(ntohs(remote.sin_port));
    strcpy( socket->ip(), inet_ntoa(remote.sin_addr));
   
    return socket;
}

void Acceptor::SetReuseAddr()
{
    int on = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
}

struct event* Acceptor::event() const
{
    return event_;
}

void Acceptor::set_event(struct event *e)
{
    event_ = e;
}
