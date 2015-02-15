// client.h (2015-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_CLIENT_H_
#define _LDB_CLIENT_H_

#include <stdio.h>
#include <vector>
#include <util/protocol.h>
#include <net/socket.h>

class Server;
class Slave;

class Client {
public:
    Client(Server *server, Socket *link, std::vector<Slave *> slaves): 
        server_(server),
        link_(link),
        slaves_(slaves),
        data_pos_(0), 
        write_pos_(0), 
        data_one_(false), 
        body_len_(0), 
        key_len_(0), 
        replay_len_(0), 
        big_recv_(NULL), 
        big_value_(NULL), 
        cmd_(-1),
        first_to_slave_(false),
        done_(false),
        client_flag_(-1){}

    ~Client() { 
        if (link_ != NULL) {
            delete link_;
        }

        if (big_recv_ != NULL) {
            delete big_recv_;
        }
        
        if (big_value_ != NULL) {
            delete big_value_;
        }
    }

    int fd() const {
        return link_->fd();
    }
    
    int Read();
    int Write();
        
    int ProcessCmd();

public:
    Server *server_;
    Socket *link_;
    int body_len_;      //包体的长度
    char head_to_slave_[HEAD_LEN];
    char recv_[MAX_PACKET_LEN];    //接收缓冲区,足够放下一个数据包
    char *big_recv_;
    
    bool done_;

private:
    //ok: return 0
    //error: return -1
    int SetCommand();
    int GetCommand();
    int DelCommand();

    //包头/包体 解析完毕: return 0
    //error: return -1, 调用者要把client 关掉
    //client exit: return 1, 调用者要把client 关掉
    //return 2,包头/包体 不完整，放回继续读
    int ReadHead();
    int ReadBody(/*Slave *slave*/);

    //error: return -1
    //ok: return 0
    //return 2, 代表没有写完
    int WritePacket();

    void WriteToSlave();
    void AddWriteEvent();

private:

    std::vector<Slave *> slaves_;

    char key_[MAX_KEY_LEN];
    char val_[MAX_VAL_LEN];

    int data_pos_;       //如果服务器实际读到的字节小于需要的字节数，
    //本字段用于记录实际读到的字节，
    //下次读取将从这里开始

    int write_pos_;     //如果服务器实际写的字节小于需要写的字节数，
    //本字段用于记录实际已经写的字节，
    //下次写时将从这里开始

    bool data_one_;     //数据包头是否读够

    int key_len_;       //key 的长度

    int replay_len_;
    char replay_[MAX_PACKET_LEN];  //回复缓冲区

    char *big_value_;

    int cmd_;
    
    short client_flag_; 
    
    bool first_to_slave_;

};


#endif
