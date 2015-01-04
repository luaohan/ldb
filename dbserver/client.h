// client.h (2015-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_CLIENT_H_
#define _LDB_CLIENT_H_

#include "command.h"
#include "../net/acceptor.h"

class Client {

    public:
        Client(Acceptor *link):link_(link){ }
        ~Client(){ }

    public:
        Acceptor *link_;

        int argc_;        //客户端发来的命令的参数的个数
        char *argv_[10];  //客户端发来的命令的参数
        
        struct Command *cmd;

        enum { BUFFER_SIZE = 2048 };
        char recv_[BUFFER_SIZE];    //接收缓冲区
        char replay_[BUFFER_SIZE];  //回复缓冲区
};


#endif
