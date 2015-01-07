// client.h (2015-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_CLIENT_H_
#define _LDB_CLIENT_H_

#include "command.h"
#include "../net/acceptor.h"

class Client {

    public:
        Client(Acceptor *link):link_(link){ }
        ~Client(){ 
            if (link_ != NULL) {
                delete link_;
            }
        }

    public:
        Acceptor *link_;

        int argc_;        //客户端发来的命令的参数的个数
        char *argv_[10];  //客户端发来的命令的参数,目前先只允许10 个吧
        
        struct Command *cmd;

        enum { BUFFER_SIZE = 2048 };
        char recv_[BUFFER_SIZE];    //接收缓冲区,这里也不太好
        char replay_[BUFFER_SIZE];  //回复缓冲区
};


#endif
