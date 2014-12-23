// ldb_client.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_CLIENT_H_
#define _LDB_CLIENT_H_

#include <vector>

class Client {

    public:
        Client(int fd):fd_(fd){ };
        ~Client(){ };

    public:

        int fd_;                    //客户端的fd

        int argc_;                  //客户端发来的命令的参数的个数
        std::vector<char *> argv_;  //客户端发来的命令的参数
        
        enum { BUFFER_SIZE = 2048 };
        char recv_[BUFFER_SIZE];    //接收缓冲区
        char replay_[BUFFER_SIZE];  //回复缓冲区

};


#endif
