// socket.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>

#include <event/event.h>

namespace ldb {
namespace net {

class Socket {
public:
    Socket(ldb::event::Loop *loop);
    Socket(int fd, const std::string &ip, int port, ldb::event::Loop *loop);
    ~Socket();

    enum {
        kRead = 0,
        kWrite = 1
    };

    typedef void (*Handler)(void *owner, int event);

    void Close();
    //ok: return 0
    //error: return -1
    bool Connect(const char *ip, int port);

    int fd() const { return fd_; }
    int port() const { return port_; }
    std::string ip() const { return ip_; }

    //if uses these, call before Connect()
    int SetNoblock();
    void SetReuseAddr(); 
    void SetLinger();
    void SetNoNagle();
    void SetRcvBuf(int size);
    void SetSndBuf(int size);

    //just like read() and write()
    //ok: return the size of read
    //error: return -1
    //a client exit: return 0

    int Read(char *buffer, int buffer_size);
    int Write(char *buffer, int buffer_size);

    void SetHandler(void *owner, Handler handler);

private:
    static void Notify(int fd, int events, void *arg);
    void Process(int fd, int events);

private:
    int fd_;
    std::string ip_;
    int port_;
    ldb::event::Loop *loop_;
    ldb::event::Event event_;

    void *owner_;
    Handler handler_;

private:
    //No copying allowed
    Socket(const Socket &);
    void operator=(const Socket &);
};

} /*namespace ldb*/
} /*namespace net*/

#endif

