// connection.h (2014-12-22)
// WangPeng (1245268612@qq.com)

#ifdef _CONNECION_H_
#define _CONNECION_H_

#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

class Connection {

    public:

        Connection();
        ~Connection();





    private:

        int fd_;
        bool noblock_;
        char remote_ip_[INET_ADDRSTRLEN];
        int remote_port_;

        static int min_recv_buf_;
        static int min_send_buf_;

};








#endif
