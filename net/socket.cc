// socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include "socket.h"

bool Socket::SetReuseAddr()
{
    int on = 1;
    int rc = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc == -1) {
        return false;
    }

    return true;
}

bool Socket::SetRcvBuf(int size)
{
    int rc = setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));
    if (rc == -1) {
        return false;
    }

    return true;
}

bool Socket::SetSndBuf(int size)
{
    int rc = setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));
    if (rc == -1) {
        return false;
    }

    return true;
}

bool Socket::SetNoNagle()
{
    int opt = 1;
    int rc = setsockopt(fd_, IPPROTO_TCP, 1, &opt, sizeof(opt));
    if (rc == -1) {
        return false;
    }

    return true;
}

