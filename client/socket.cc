// socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "socket.h"

Socket::Socket(): 
    fd_(-1), port_(-1), backlog_(-1), is_noblocked_(false) 
{
    fd_ = socket( AF_INET, SOCK_STREAM, 0 );
    
    ip_[0] = '\0';
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

int Socket::readData(char *buffer, int buffer_size)
{
    int ret = 0;
    int want = buffer_size;
    while(want  > 0)
    {
        int len = read(fd_, buffer, want);
        if(len == -1){
            if(errno == EINTR){

                continue;
            }else if(errno == EWOULDBLOCK){
                break;
            }else{
                return -1;
            }
        }else{
            if(len == 0){
                return 0;
            }

            ret += len;
            buffer += len;
        }       
        if( !is_noblocked_ ){
            break;
        }
    }

    return ret;
}

int Socket::writeData(char *buffer, int buffer_size)
{
    int ret = 0;
    int want = buffer_size;
    while( want > 0 )
    {
        int len = write(fd_, buffer, want);
        if(len == -1){
            if(errno == EINTR){
                continue;
            }else if(errno == EWOULDBLOCK){
                break;
            }else{
                return -1;
            }
        }else{
            if(len == 0){
                break;
            }
            ret += len;
            buffer += len;
        }
        if(!is_noblocked_){
            break;
        }
    }

    return ret;
}
