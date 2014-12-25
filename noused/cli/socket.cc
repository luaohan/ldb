// socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "socket.h"

Socket::Socket(bool local): 
    fd_(-1), port_(-1), backlog_(-1), is_noblocked_(false) 
{
    ip_[0] = '\0';

    if ( local ) {
        fd_ = socket( AF_INET, SOCK_STREAM, 0 );
    }
}

Socket::~Socket()
{
    if ( fd_ > 0 ) close(fd_);
}

int Socket::getFd() const
{
    return fd_;
}


int Socket::getBacklog()
{ 
    return backlog_; 
}

int Socket::getPort()
{
    return port_;
}

char *Socket::getIp()
{
    return ip_;
}

int Socket::setNoblock()
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

bool Socket::isNoblocked()
{
    return is_noblocked_;
}

int Socket::Connect(const char *ip, int port)
{
    struct sockaddr_in addr;
    int result;
    int on = 1;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    result = connect(fd_, (struct sockaddr *) & addr, sizeof(struct sockaddr)); 
    if ( result != 0 ) {
        close(fd_);
        fd_ = -1;
        return -1;
    }
    
    strcpy(ip_, ip);
    port_ = port;

    return 0;
}

int Socket::Listen(const char *ip, int port, int backlog)
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

Socket *Socket::Accept()
{
    int fd = -1;
    struct sockaddr_in remote;
    int len = 0;

ReAccept:
    len = sizeof( struct sockaddr_in );

    fd = accept(fd_, (struct sockaddr *)&remote, (socklen_t *)&len);
    if (fd == -1) {
        int err = errno;
        if ( err == EINTR || err == EAGAIN ) {
            goto ReAccept;
        }
#ifdef  EPROTO
        if (err == EPROTO || err == ECONNABORTED) {
            goto ReAccept;
        }
#else
        if (err == ECONNABORTED ) {
            goto ReAccept;
        }
#endif
        return NULL;  
    }

    Socket *socket = new Socket(false);
    socket->fd_ = fd;
    socket->port_ = ntohs(remote.sin_port); 
    strcpy( socket->ip_, inet_ntoa(remote.sin_addr));

    return socket;
}

void Socket::setReuseAddr()
{
    int on = 1;                          
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
}

void Socket::setLinger()
{
    struct linger optval;
    optval.l_onoff = 1;
    optval.l_linger = 60;
    setsockopt(fd_, SOL_SOCKET, SO_LINGER, (char *)&optval, sizeof(struct linger));
}

void Socket::setRcvBuf(int size)
{
    setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));
}

void Socket::setSndBuf(int size)
{
    setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));
}

void Socket::setNoNagle()
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
