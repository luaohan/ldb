// socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>

#include "sync_socket.h"

int SyncSocket::Connect()
{
    assert(!ip_.empty());

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_.c_str());
    addr.sin_port = htons(port_);
    int rc = connect(fd_, (struct sockaddr *) & addr, sizeof(struct sockaddr)); 
    if (rc != 0) {
        return -1;
    }
    
    return 0;
}

int SyncSocket::Read(char *buffer, int len) 
{
    assert(buffer != NULL);
    assert(len > 0);

    int remain = len;
    while (remain > 0) {
        int bytes = read(fd_, buffer, remain);
        if (bytes < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        } else if (bytes == 0) {
            return 0;
        }

        remain -= bytes;
        buffer += bytes;
    }
    
    return len;
}

int SyncSocket::Write(char *buffer, int len) 
{
    assert(buffer != NULL);
    assert(len > 0);

    int remain = len;
    while (remain > 0) {
        int bytes = write(fd_, buffer, remain);
        if (bytes < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        }

        remain -= bytes;
        buffer += bytes;
    }

    return len;
}

void SyncSocket::Close() 
{
    if (fd_ > 0) {
        close(fd_);
        fd_ = -1;
    }
}

