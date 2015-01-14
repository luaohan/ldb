// client.cc (2015-01-09)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "client.h"
#include "command.h"
#include "../util/protocol.h"
#include "../util/log.h"

#if 0
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

    if (packet_type == SET_CMD) {
        cmd_ = SetCommand;

    } else if (packet_type == GET_CMD) {
        cmd_ = GetCommand;

    } else if (packet_type == DEL_CMD) {
        cmd_ = DelCommand;

    } else {
        //error cmd
        log_error("error cmd, fd[%d], port[%d], ip[%s]",
                link_->GetFd(), link_->GetPort(), link_->GetIp());

        return -1;
    }

    body_len_ = packet_len - HEAD_LEN;

    data_one_ = true;
    data_pos_ = 0; 

    return 0;
}

int Client::ReadBody()
{
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

    if (body_len_ - key_len - sizeof(short) > 0) {
        memcpy(val_, recv_ + sizeof(short) + key_len, 
                body_len_ - key_len - sizeof(short));
    }

    data_one_ = false;
    data_pos_ = 0;

    return 0;
}

int Client::WritePacket()
{
    int ret = link_->WriteData(replay_ + write_pos_, replay_len_ - write_pos_);

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

    return 0;
}

#endif

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

    if (packet_type == SET_CMD) {
        cmd_ = SetCommand;

    } else if (packet_type == GET_CMD) {
        cmd_ = GetCommand;

    } else if (packet_type == DEL_CMD) {
        cmd_ = DelCommand;

    } else {
        //error cmd
        log_error("error cmd, fd[%d], port[%d], ip[%s]",
                link_->GetFd(), link_->GetPort(), link_->GetIp());

        return -1;
    }

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
