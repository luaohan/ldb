// client.h (2015-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_CLIENT_H_
#define _LDB_CLIENT_H_

#include <stdio.h>

#include "command.h"
#include "../net/socket.h"

class Client {

    public:
        Client(Socket *link):link_(link), data_one_pos_(0), data_one_(false), 
            data_two_pos_(0), data_two_(false){ }

        ~Client(){ 
            if (link_ != NULL) {
                delete link_;
            }
        }

    public:
        Socket *link_;

        int argc_;        //客户端发来的命令的参数的个数
        char *argv_[10];  //客户端发来的命令的参数,目前先只允许10 个吧
        
        struct Command *cmd;

        int data_one_pos_;   //如果服务器实际读到的字节小于需要的字节数，
                             //本字段用于记录实际读到的字节，
                             //下次读取将从这里开始
        int data_two_pos_;

        bool data_one_; //数据包头是否读够
        bool data_two_; //数据包体是否读够

        char recv_[BUFSIZ];    //接收缓冲区,这里也不太好
        char replay_[BUFSIZ];  //回复缓冲区

};


#endif
