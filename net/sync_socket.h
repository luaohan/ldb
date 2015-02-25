// socket.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SYNC_SOCKET_H_
#define _SYNC_SOCKET_H_

#include "socket.h"

class SyncSocket : public Socket {
public:
    SyncSocket(const std::string &ip, int port)
        : Socket(ip, port) {}
    SyncSocket(int fd, const std::string &ip, int port) 
        : Socket(fd, ip, port) {}
    virtual ~SyncSocket() { Close(); }

    virtual int Connect();
    virtual int Read(char *buffer, int len);
    virtual int Write(char *buffer, int len);
    virtual void Close();
};

#endif // _SYNC_SOCKET_H_
