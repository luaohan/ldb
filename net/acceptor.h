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
        Acceptor(bool local = true);
        ~Acceptor();

        //ok: return 0
        //error: return -1
        int Listen(const char *ip, int port, int backlog);
       
        //error: return NULL
        Acceptor *Accept();
       
        void Close();
        int getBacklog();
        int getFd() const;
        int getPort();
        char *getIp(); //error: return NULL
       
        bool isNoblocked();
       
        //if uses these, call before Listen()
        int setNoblock();
        void setReuseAddr();
        void setLinger();
        void setRcvBuf(int size);
        void setSndBuf(int size);
        void setNoNagle();
       
        //just like read() and write()
        //ok: return the size of read
        //error: return -1
        //a client exit: return 0
        int readData(char *buffer, int buffer_size);
        int writeData(char *buffer, int buffer_size);
    
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
