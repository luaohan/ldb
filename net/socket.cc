// socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "socket.h"

Socket::Socket(): 
    fd_(-1), port_(-1), is_noblocked_(false), event_(NULL)
{
    ip_[0] = '\0';
    
    fd_ = socket( AF_INET, SOCK_STREAM, 0 );

    assert(fd_ != -1);
    
}

Socket::Socket(const char *ip, int port): 
    fd_(-1), port_(port), is_noblocked_(false), event_(NULL)
{
    strcpy(ip_, ip);
    
    fd_ = socket( AF_INET, SOCK_STREAM, 0 );

    assert(fd_ != -1);
    
}

Socket::~Socket()
{
    if (event_ != NULL) {
        event_free(event_);
    }

    Close();
}

void Socket::Close() 
{
    if (fd_ > 0) {
        close(fd_);
        fd_ = -1;
    }
}

int Socket::fd() const
{
    return fd_;
}

int Socket::port() const
{
    return port_;
}

char *Socket::ip() 
{
    return ip_;
}

int Socket::SetNonBlock()
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

bool Socket::IsNoblocked() const
{
    return is_noblocked_;
}

int Socket::Connect(const char *ip, int port)
{
    struct sockaddr_in addr;
    int result;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    result = connect(fd_, (struct sockaddr *) & addr, sizeof(struct sockaddr)); 
    if ( result != 0 ) {
        //Close();
        return -1;
    }
    
    strcpy(ip_, ip);
    port_ = port;

    return 0;
}

int Socket::Connect()
{
    struct sockaddr_in addr;
    int result;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_);
    addr.sin_port = htons(port_);
    result = connect(fd_, (struct sockaddr *) & addr, sizeof(struct sockaddr)); 
    if ( result != 0 ) {
        //Close();
        return -1;
    }
    
    return 0;
}

void Socket::SetReuseAddr()
{
    int on = 1;                          
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

int Socket::ReadData(char *buffer, int size)
{
    assert(buffer != NULL);
    assert(size > 0);
    assert(is_noblocked_);

read_again:
    int read_size = read(fd_, buffer, size);
    if (read_size < 0) {
        if (errno == EINTR) {
            goto read_again;
        }

        return -1;
    }

    return read_size;
}

int Socket::WriteData(char *buffer, int size)
{
    assert(buffer != NULL);
    assert(size > 0);
    assert(is_noblocked_);

write_again:
    int write_size = write(fd_, buffer, size);
    if (write_size == -1) {
        if (errno == EINTR) {
            goto write_again;
        }

        return -1;
    }

    return write_size;
}

int Socket::BlockRead(char *buffer, int size) 
{
    assert(buffer != NULL);
    assert(size > 0);
    assert(!is_noblocked_);

    int nleft = size;
    int nread;

    while (nleft > 0) {
        nread = read(fd_, buffer, nleft);
        if (nread < 0) {
            if (errno == EINTR) {
                //nread = 0;
                continue;
            } else {
                return -1;
            }
        } else if (nread == 0) {
            //break;
            return 0;
        }

        nleft -= nread;
        buffer += nread;
    }
    //return size - nleft;
    return size;
}

int Socket::BlockWrite(char *buffer, int size) 
{
    assert(buffer != NULL);
    assert(size > 0);
    assert(!is_noblocked_);

    int nleft = size;
    int nwrite;

    while (nleft > 0) {
        nwrite = write(fd_, buffer, nleft);
        if (nwrite <= 0) {
            if (nwrite < 0 && errno == EINTR) {
                //nwrite = 0;
                continue;
            } else {
                return -1;
            }
        }

        nleft -= nwrite;
        buffer += nwrite;
    }

    return size;
}

struct event* Socket::event() const{
    return event_;
}

void Socket::set_event(struct event *e)
{
    event_ = e;
}
