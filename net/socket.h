// socket.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>

namespace ldb {
namespace net {

class Socket {
public:
    Socket();
    Socket(int fd, const std::string &ip, int port);
    ~Socket();

    void Close();
    //ok: return 0
    //error: return -1
    int Connect(const char *ip, int port);

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
    int ReadData(char *buffer, int buffer_size);
    int WriteData(char *buffer, int buffer_size);

private:
    int fd_;
    std::string ip_;
    int port_;

private:
    //No copying allowed
    Socket(const Socket &);
    void operator=(const Socket &);
};

} /*namespace ldb*/
} /*namespace net*/

#endif

