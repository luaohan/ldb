// socket.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _ASYNC_SOCKET_H_
#define _ASYNC_SOCKET_H_

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <event2/event.h>

#include "socket.h"

class AsyncSocket : public Socket {
public:
    enum {
        kRead = 0,
        kWrite,
        kConnected,
        kTimeout
    };

    typedef void (*Handler)(int event, void *data);

    AsyncSocket(struct event_base *base, 
            const std::string &ip, int port)
        : Socket(ip, port), 
        base_(base), 
        handler_(NULL), 
        data_(NULL), 
        connecting_(false),
        event_(NULL) {}

    AsyncSocket(struct event_base *base, 
            int fd, const std::string &ip, int port)
        : Socket(fd, ip, port), 
        base_(base), 
        handler_(NULL),
        data_(NULL),
        connecting_(false),
        event_(NULL)  {}

    virtual ~AsyncSocket() { Close(); }

    void SetHandler(Handler handler, void *data);
    virtual int Connect();
    void Start();
    void Close();

    
    int Read(char *buffer, int len);
    int Write(char *buffer, int len);

    bool SetNonBlock();

private:

    static void Notify(int fd, short event, void *data);

    void OnTimeout();
    void OnConnect();
    void OnRead();
    void OnWrite();

private:
    struct event_base *base_;
    Handler handler_;
    void *data_;

    bool connecting_;
    struct event *event_;
};

#endif //_ASYNC_SOCKET_H_

