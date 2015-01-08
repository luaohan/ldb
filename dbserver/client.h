// client.h (2015-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_CLIENT_H_
#define _LDB_CLIENT_H_

#include <stdio.h>

#include "command.h"
#include "../util/protocol.h"
#include "../net/socket.h"

class Client {

    public:
        Client(Socket *link):link_(link), data_pos_(0), data_one_(false), 
             body_len_(0), cmd(NULL){ }

        ~Client(){ 
            if (link_ != NULL) {
                delete link_;
            }
        }

    public:
        Socket *link_;

        char key_[MAX_KEY_LEN];
        char val_[MAX_VAL_LEN];
        
        //struct Command *cmd;
        command_proc *cmd;
        int data_pos_;       //如果服务器实际读到的字节小于需要的字节数，
                             //本字段用于记录实际读到的字节，
                             //下次读取将从这里开始

        bool data_one_; //数据包头是否读够

        int body_len_;  //包体的长度
        int key_len_;   //key 的长度

        char recv_[MAX_PACKET_LEN];    //接收缓冲区,足够放下一个数据包
        char replay_[MAX_PACKET_LEN];  //回复缓冲区

};


#endif
