// socket.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>

class Socket {
public:
    Socket(const std::string &ip, int port) 
        : fd_(-1), 
        ip_(ip),
        port_(-1) {}
    Socket(int fd, const std::string &ip, int port) 
        : fd_(fd), ip_(ip), port_(port) {}
    virtual ~Socket() {}

    virtual int Connect() = 0;
    virtual int Read(char *buffer, int len) = 0;
    virtual int Write(char *buffer, int len) = 0;
    virtual void Close() = 0;
    
    int fd() const { return fd_; }
    int port() const { return port_; }
    const std::string &ip() { return ip_; }

    bool SetReuseAddr(); 
    bool SetNoNagle();
    bool SetRcvBuf(int len);
    bool SetSndBuf(int len);

private:
    Socket(const Socket &);
    void operator=(const Socket &);

protected:
    int fd_;
    std::string ip_;
    int port_;
};

#endif

