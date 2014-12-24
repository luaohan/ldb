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
        Socket();
        ~Socket();

        int set_fd_noblock();
        
        int Connect(const char *ip, int port);
        int Listen(const char *ip, int port);
        int Accept();

        int read_data();
        int write_data();
        
    private:
        int fd_;
        bool fd_blocked_;
        char *ip_;
        int port_;
    
        char input_[2048];
        char output_[2048];

};



#endif
