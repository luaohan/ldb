// client.cc (2015-01-09)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <leveldb/slice.h>

#include "client.h"
#include "server.h"
#include "../util/protocol.h"
#include "../util/log.h"

int Client::ReadHead()
{
    int ret = link_->ReadData(recv_ + data_pos_, HEAD_LEN - data_pos_);

    if (ret < HEAD_LEN - data_pos_) {  
        if (ret == 0) {
            log_info("a clent exit, fd[%d], port[%d], ip[%s]",
                    link_->GetFd(), link_->GetPort(), link_->GetIp());

            return 1;
        }

        if (ret < 0) {
            if (errno != EAGAIN) {
                log_error("read data error, fd[%d], port[%d], ip[%s]",
                        link_->GetFd(), link_->GetPort(), link_->GetIp());

                return -1;
            }

            ret = 0;
        }

        data_pos_ += ret;
        return 2;
    } 

    //到这里说明需要的东西已经读够
    //解析读到的内容
    int packet_len = ntohl(*((int *)&(recv_[0])));
    short packet_type = ntohs(*((short *)&(recv_[sizeof(int)])));

    if (packet_type != SET_CMD 
            && packet_type != GET_CMD
            && packet_type != DEL_CMD) {
        //error cmd
        log_error("error cmd, fd[%d], port[%d], ip[%s]",
                link_->GetFd(), link_->GetPort(), link_->GetIp());

        return -1;
    }

    cmd_ = packet_type;

    body_len_ = packet_len - HEAD_LEN;

    data_one_ = true;
    data_pos_ = 0; 

    return 0;
}

int Client::ReadBody()
{
    if (body_len_ <= ONE_M) {

        int ret = link_->ReadData(recv_ + data_pos_, body_len_ - data_pos_);
        if (ret < body_len_ - data_pos_) {
            if (ret == 0) {
                log_info("a clent exit, fd[%d], port[%d], ip[%s]",
                        link_->GetFd(), link_->GetPort(), link_->GetIp());

                return 1;
            }

            if (ret < 0) {
                if (errno != EAGAIN) {
                    log_error("read data error, fd[%d], port[%d], ip[%s]",
                            link_->GetFd(), link_->GetPort(), link_->GetIp());

                    return -1;
                }

                ret = 0;
            }

            data_pos_ += ret;
            return 2;
        }

        //到这里说明需要的东西已经读够
        //解析读到的内容, 放到cli 的相应成员
        //short key_len = ntohs(*((short *)&(recv_[0])));
        int key_len = ntohs(*((short *)&(recv_[0])));
        key_len_ = key_len;
        
        if (key_len > 0) {
            memcpy(key_, recv_ + sizeof(short), key_len);
        }
        
        int value_len = body_len_ - key_len - sizeof(short);

        if ( value_len > 0) {
            memcpy(val_, recv_ + sizeof(short) + key_len, value_len);
        }

        data_one_ = false;
        data_pos_ = 0;

        return 0;
    }

    if (big_recv_ == NULL) {
        big_recv_ = (char *)malloc(body_len_);
        if (big_recv_ == NULL) {
            return -1;
        }
    }

    int ret = link_->ReadData(big_recv_ + data_pos_, body_len_ - data_pos_);
    if (ret < body_len_ - data_pos_) {
        if (ret == 0) {
            log_info("a clent exit, fd[%d], port[%d], ip[%s]",
                    link_->GetFd(), link_->GetPort(), link_->GetIp());

            return 1;
        }

        if (ret < 0) {
            if (errno != EAGAIN) {
                log_error("read data error, fd[%d], port[%d], ip[%s]",
                        link_->GetFd(), link_->GetPort(), link_->GetIp());

                return -1;
            }

            ret = 0;
        }

        data_pos_ += ret;
        return 2;
    }

    //到这里说明需要的东西已经读够
    //解析读到的内容, 放到cli 的相应成员
    //short key_len = ntohs(*((short *)&(recv_[0])));
    int key_len = ntohs(*((short *)&(big_recv_[0])));
    key_len_ = key_len;
    
    if (key_len > 0) {
        memcpy(key_, big_recv_ + sizeof(short), key_len);
    }

    int value_len = body_len_ - key_len - sizeof(short);
    if (value_len > 0) {
        if (big_value_ == NULL) {
            big_value_ = (char *)malloc(value_len);
            if (big_value_ == NULL) {
                return -1;
            }
        }

        memcpy(big_value_, big_recv_ + sizeof(short) + key_len, value_len);
    }

    data_one_ = false;
    data_pos_ = 0;

    free(big_recv_);
    big_recv_ = NULL;

    return 0;
}

int Client::WritePacket()
{
    int ret;
    if (big_value_ == NULL) {
        ret = link_->WriteData(replay_ + write_pos_, replay_len_ - write_pos_);
    } else {
        ret = link_->WriteData(big_value_ + write_pos_, replay_len_ - write_pos_);
    }

    if (ret < replay_len_ - write_pos_) {  
        if (ret == 0) {
            log_error("write error, fd[%d], port[%d], ip[%s]",
                    link_->GetFd(), link_->GetPort(), link_->GetIp());

            return -1;
        }

        if (ret < 0) {
            if (ret != EAGAIN) {
                log_error("read data error, fd[%d], port[%d], ip[%s]",
                        link_->GetFd(), link_->GetPort(), link_->GetIp());

                return -1;
            }

            ret = 0;
        }

        write_pos_ += ret;

        return 2;
    } 

    //到这里说明所有的东西已经写完
    //解析读到的内容

    write_pos_ = 0;
   
    if (big_value_ != NULL) {
        free(big_value_);
        big_value_ = NULL;
    }

    return 0;
}

int Client::SetCommand() 
{
    leveldb::Slice key(key_, key_len_);

    int ret;
    int value_len = body_len_ - key_len_ - sizeof(short);

    if (big_value_ == NULL) {

        leveldb::Slice val(val_, value_len);
        ret = server_->Insert(key, val);
        if (ret == -1) {
            //fatal error
            log_fatal("insert error");
            return -1;
        }   
    } else {
        leveldb::Slice val(big_value_, value_len);
        ret = server_->Insert(key, val);
        if (ret == -1) {
            //fatal error
            log_fatal("insert error");
            return -1;
        }   
        free(big_value_);
        big_value_ = NULL;
    }

    ret = FillPacket(replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_OK);
    replay_len_ = ret;
    ret = WritePacket();
    
    if (ret == 2) { // 没写完
        //将写事件加入epoll
        Event e;
        e.fd_ = link_->GetFd();
        e.ptr_ = this;
        
        server_->event_.AddWriteEvent(e);
    } else if (ret == -1) { //error
        return -1;
    }

    return 0;
}

int Client::GetCommand()
{
    leveldb::Slice key(key_, key_len_);

    std::string val;
    int ret = server_->Get(key, &val);
    if (ret < 0) {
        //key not exist 
        ret = FillPacket(replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_NO_THE_KEY);
    } else {
        //key exist
        //如果value 大于ONE_M, 需要申请内存
        if (val.size() <= ONE_M) {

            ret = FillPacket(replay_, MAX_PACKET_LEN, val.c_str(), val.size(), 
                    NULL, 0, REPLAY_OK);
    
        } else {
            int size = val.size() + sizeof(int) + sizeof(short) * 2;
            if (big_value_ == NULL) {
                big_value_ = (char *)malloc(size);
                if (big_value_ == NULL) {
                    return -1;
                }
            }

            ret = FillPacket(big_value_, size, NULL, val.size(), 
                    NULL, 0, REPLAY_OK);
            memcpy(big_value_ + sizeof(int) + sizeof(short) * 2, 
                    val.c_str(), val.size());
        
        }
    }

    replay_len_ = ret;
    ret = WritePacket();

    if (ret == 2) { // 没写完

        //将写事件加入epoll
        Event e;
        e.fd_ = link_->GetFd();
        e.ptr_ = this;
        
        server_->event_.AddWriteEvent(e);
    } else if (ret == -1) { //error
        return -1;
    }

    return 0;
}

int Client::DelCommand()
{
    leveldb::Slice key(key_, key_len_);
    server_->Delete(key);

    int ret = FillPacket(replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_OK);
    replay_len_ = ret;
    ret = WritePacket();

    if (ret == 2) { // 没写完
        //将写事件加入epoll
        Event e;
        e.fd_ = link_->GetFd();
        e.ptr_ = this;
        
        server_->event_.AddWriteEvent(e);
    } else if (ret == -1) { //error
        return -1;
    }
    
    return 0;
}

int Client::Read()
{
    //读数据包
    int ret;
    if (data_one_ == false) {
        
        ret = ReadHead();  //读包头
        if (ret == 1 || ret == -1) {
            return -1;
        } else if (ret == 2) {  //包头不够
            return 0;
        }
    }

    ret = ReadBody(); //读包体
    if (ret == 1 || ret == -1) {
        return -1;
    } else if (ret == 2) { //包体不够
        return 0;
    }

    ret = ProcessCmd();
    if (ret == -1) {
        return -1;
    }

    return 0;
}

int Client::ProcessCmd()
{
    switch (cmd_) {
        case SET_CMD:
            return SetCommand();
        break;
        case GET_CMD:
            return GetCommand();
        break;
        case DEL_CMD:
            return DelCommand();
        break;
        default:
            return -1;
        break;
    }

    return -1;
}

int Client::Write()
{
    int ret = WritePacket();
    if (ret == -1) {
        return -1;
    } else if (ret == 2) { //还没写完
        return 2;
    }
}

