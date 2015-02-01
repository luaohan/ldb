// acceptor.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <event2/event.h>

class Socket;

class Acceptor {

    public:
        Acceptor();
        ~Acceptor();

        //ok: return 0
        //error: return -1
        int Listen(const char *ip, int port, int backlog);
       
        //error: return NULL
        Socket *Accept();
        
        void Close();

        int backlog() const;
        int fd() const;
        int port() const;
       
        bool IsNoblocked() const;
       
        int SetNonBlock();
        //if uses these, call before Listen()
        void SetReuseAddr();

        struct event *event() const;
        void set_event(struct event *e);
    
    private:
        int fd_;
        char ip_[INET_ADDRSTRLEN];
        int port_;
        int backlog_;
        bool is_noblocked_;
        
        struct event *event_;
        
    private:
        //No copying allowed
        Acceptor(const Acceptor &);
        void operator=(const Acceptor &);
};

#endif
