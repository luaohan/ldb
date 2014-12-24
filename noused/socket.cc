// socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include "socket.h"
#include ""

Socket::Socket():fd_(-1), fd_blocked_(true), port_(-1)
{

}

Socket::~Socket()
{
    if ( fd_ > 0 ) close(fd_);
}

int Socket::set_fd_noblock()
{
    int flags;

    if ((flags = fcntl(fd_, F_GETFL, NULL)) < 0) {        
        return -1;
    }

    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1; 
    }   

    return 0;
}

int Socket::Connect(const char *ip, int port)
{
    struct sockaddr_in addr;
    int result;
    int on = 1;
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if ( fd == -1 ) {
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    result = connect(fd, (struct sockaddr *) & addr, sizeof(struct sockaddr)); 
    if ( result != 0 ) {
        close(fd);
        fd = -1;
        return -1;
    }
    
    ip_ = ip;
    port_ = port;
    fd_ = fd;

    return fd;
}

int Sockete::Listen(const char *ip, int port)
{
    assert(ip != NULL);

    struct sockaddr_in addr;
    int s = socket( AF_INET, SOCK_STREAM, 0 );
    if ( s == -1 ) {
        return -1;
    }

    addr.sin_family = AF_INET;
    if (strcmp(ip, "0.0.0.0") == 0) {
        addr.sin_addr.s_addr = INADDR_ANY; //htonl(INADDR_ANY);
    } else {
        addr.sin_addr.s_addr = inet_addr( ip );
    }

    addr.sin_port = htons( port );

    if (bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1 ) {
        close( s);
        s = -1;
        return -1;
    }

    if (listen(s, IKV_MAX_LISTEN_QUEUE ) == -1 ) {
        close(s);
        s = -1;
        return -1;
    }
    
    ip_ = ip;
    port_ = port;
    fd_ = s;

    return s;
}

int Socket::Accept()
