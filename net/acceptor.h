// acceptor.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <event2/event.h>
#include <string>

class Socket;

class Acceptor {
public:
    typedef void (*Handler)(void *data);

    Acceptor(struct event_base *base, Handler handler, void *data)
        : base_(base), handler_(handler), data_(data) {}
    ~Acceptor() { Close(); }

    int Listen(const std::string &ip, int port);
    AsyncSocket *Accept();
    void Close();

    int fd() const { return fd_; }
    const std::string &ip() { return ip_; }
    int port() const { return port_; }

private:
    Acceptor(const Acceptor &);
    void operator=(const Acceptor &);

    bool SetNonBlock();
    bool SetReuseAddr();

    static void Notify(int fd, short what, void *data);

private:
    struct event_base *base_;
    struct event *event_;

    int fd_;
    std::string ip_;
    int port_;
    void *data_;
    Handler handler_;
};

#endif
