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
        Socket(const char *ip, int port);
        ~Socket();

        void Close();
        //ok: return 0
        //error: return -1
        int Connect(const char *ip, int port);
        int Connect();

        void set_fd(int fd) {
            fd_ = fd;
        }
        
        void set_port(int port) {
            port_ = port;
        }

        int fd() const;
        int port() const;
        char *ip(); 

        bool IsNoblocked() const;
    
        //if uses these, call before Connect()
        int set_noblock();
        void set_reuseAddr(); 
        void set_linger();
        void set_no_nagle();
        void set_rcv_buf(int size);
        void set_snd_buf(int size);

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

    private:
        int fd_;
        char ip_[INET_ADDRSTRLEN];
        int port_;
        bool is_noblocked_;

    private:
        //No copying allowed
        Socket(const Socket &);
        void operator=(const Socket &);
};



#endif
