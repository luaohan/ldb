// acceptor.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>

class Socket;

class Acceptor {

    public:
        Acceptor();
        ~Acceptor();

        int Listen(const std::string &ip, int port, int backlog);
        Socket *Accept();
        void Close();
        
        int fd() const;
    
        int SetNonBlock();
        void SetReuseAddr(); 

    private:
        int fd_;
        std::string ip_;
        int port_;
        int backlog_;

    private:
        Acceptor(const Acceptor &);
        void operator=(const Acceptor &);
};

#endif

