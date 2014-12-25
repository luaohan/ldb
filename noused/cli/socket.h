// socket.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

class Socket {

    public:
        Socket(bool local);
        ~Socket();

        //ok: return 0
        //error: return -1
        int Connect(const char *ip, int port);

        //ok: return 0
        //error: return -1
        int Listen(const char *ip, int port, int backlog);

        Socket *Accept();
        
        int getBacklog();
        int getFd() const;
        int getPort();
        char *getIp(); //error: return NULL

        bool isNoblocked();
    
        //if uses these, call before Listen() or Connect()
        int setNoblock();
        void setReuseAddr(); 
        void setLinger();
        void setRcvBuf(int size);
        void setSndBuf(int size);
        void setNoNagle();
        
    private:
        
        int fd_;
        char ip_[INET_ADDRSTRLEN];
        int port_;
        int backlog_;
        bool is_noblocked_;
};



#endif
