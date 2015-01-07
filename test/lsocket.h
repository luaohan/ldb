// socket.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

class Socket {

    public:
        Socket();
        ~Socket();

        //ok: return 0
        //error: return -1
        int Connect(const char *ip, int port);

        int getBacklog();
        int getFd() const;
        int getPort();
        char *getIp(); 

        bool isNoblocked();
    
        //if uses these, call before Connect()
        int setNoblock();
        void setReuseAddr(); 
        void setLinger();
        void setRcvBuf(int size);
        void setSndBuf(int size);
        void setNoNagle();

        //just like read() and write()
        int readData(char *buffer, int buffer_size);
        int writeData(char *buffer, int buffer_size);
        
    private:
        
        int fd_;
        char ip_[INET_ADDRSTRLEN];
        int port_;
        int backlog_;
        bool is_noblocked_;
};



#endif
