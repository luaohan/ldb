// socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "socket.h"

namespace ldb {
namespace net {

Socket::Socket(): 
    fd_(-1), port_(-1)
{
}

Socket::Socket(int fd, const std::string &ip, int port)
    : fd_(fd), ip_(ip), port_(port)
{
}

Socket::~Socket()
{
    Close();
}

void Socket::Close() 
{
    if (fd_ > 0) {
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

int Socket::Connect(const char *ip, int port)
{
    struct sockaddr_in addr;
    int result;
    int on = 1;

    fd_ = socket( AF_INET, SOCK_STREAM, 0 );
    if (fd_ == -1) {
        //LOG(ERROR)
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    result = connect(fd_, (struct sockaddr *) & addr, sizeof(struct sockaddr)); 
    if ( result != 0 ) {
        Close();
        return -1;
    }
    
    ip_ == ip;
    port_ = port;

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

int Socket::ReadData(char *buffer, int buffer_size)
{
    int ret = 0;
    int want = buffer_size;
    while (want  > 0) {
        int len = read(fd_, buffer, want);
        if (len == -1) {
            if(errno == EINTR){

                continue;
            }

            return -1;
        } 

        if (len == 0) {
            return 0;
        }

        ret += len;
        buffer += len;
        want -= len;
    }

    return ret;
}

int Socket::WriteData(char *buffer, int buffer_size)
{
    int ret = 0;
    int want = buffer_size;
    while ( want > 0 ) {
        int len = write(fd_, buffer, want);
        if (len == -1) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        ret += len;
        buffer += len;
        want -= len;
    }

    return ret;
}


} /*namespace ldb*/
} /*namespace net*/

