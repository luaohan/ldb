// acceptor.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

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
        
        int getBacklog() const;
        int getFd() const;
        int getPort() const;
        char *getIp() const; 

        bool isNoblocked() const;
    
        //if uses these, call before Listen() 
        int SetNoblock();
        void SetReuseAddr(); 

    private:
        int fd_;
        char ip_[INET_ADDRSTRLEN];
        int port_;
        int backlog_;
        bool is_noblocked_;

    private:
        //No copying allowed
        Acceptor(const Acceptor &);
        void operator=(const Acceptor &);
};



#endif
