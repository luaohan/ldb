// socket.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <event2/event.h>

class Socket {

    public:
        Socket();
        Socket(const char *ip, int port);
        ~Socket();

        void Close();
        //ok: return 0
        //error: return -1
        int Connect(const char *ip, int port);
        int Connect();
        
        int fd() const;
        int port() const;
        char *ip(); 

        void set_fd(int fd) {
            fd_ = fd;
        }
        
        void set_port(int port) {
            port_ = port;
        }

        bool IsNoblocked() const;
    
        //if uses these, call before Connect()
        int SetNonBlock();
        void SetReuseAddr(); 
        void SetLinger();
        void SetNoNagle();
        void SetRcvBuf(int size);
        void SetSndBuf(int size);

        //just like read() and write()
        //ok: return the size of read
        //a client exit: return 0
        //调用者需要判断 EAGAIN
        
        int ReadData(char *buffer, int size);
        int WriteData(char *buffer, int size);

        //ok: 返回读到的字节数
        //error: return -1
        //对方断开: return 0
        int BlockRead(char *buffer, int size);

        //ok: return size
        //error: return -1;
        int BlockWrite(char *buffer, int size);

        struct event* event() const;
        void set_event(struct event *e);

    private:
        int fd_;
        char ip_[INET_ADDRSTRLEN];
        int port_;
        bool is_noblocked_;

        struct event *event_;

    private:
        //No copying allowed
        Socket(const Socket &);
        void operator=(const Socket &);
};


#endif
