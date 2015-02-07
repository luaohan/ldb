// slave.cc (2015-01-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <dbserver/server.h>
#include <dbserver/client.h>
#include <dbserver/slave.h>
#include <util/log.h>

int Slave::Read()
{
    int len = PACKET_LEN_SLAVE_REPLAY - data_pos_; //8个字节
    char *buf = recv_ + data_pos_;

    int ret = link_->ReadData(buf, len);
    if (ret < len) {
        //这里ret == 0 也视为错误，因为server 与 slave 不允许断开
        if (ret == 0) {
            log_error("slave error: %s\n", strerror(errno));
            exit(0);//return -1;这里不允许错误
        }
       
        if (ret == -1) {
            if (errno != EAGAIN) {
                log_error("slave error: %s\n", strerror(errno));
                exit(0);//return -1;
            }
        
            ret == 0;
        }
        
        data_pos_ += ret;
        return 2;//没读完
    }

    //到这里表示读完了slave 发来的信息
    data_pos_ = 0;
    
    //解析slave 发来的信息
    //这里假设slave 只会成功，所以只解析cli_fd
    short cli_fd = ntohs(*((short *)&(recv_[HEAD_LEN])));  
    Client *cli = server_->FindClient(cli_fd);
    if (cli == NULL) {
        return 0;
    }
    
    if (cli->done_ == false) {
        cli->ProcessCmd();
    }

    return 0;
}

int Slave::Write()
{
    std::vector<Client *>::iterator it = clients_.begin();
    Client *cli = (*it);

    int ret;
    if (data_one_ == false) {
        //向slave 写包头
        char *buf = cli->head_to_slave_ + write_pos_;
        int len = HEAD_LEN - write_pos_;
        ret = link_->WriteData(buf, len);
        if (ret < len) {
            if (ret == 0) {
                log_error("slave error: %s\n", strerror(errno));
                exit(0);//return -1;
            }

            if (ret < 0) {
                if (errno != EAGAIN) {
                    log_error("slave error: %s\n", strerror(errno));
                    exit(0);//return -1;
                }

                ret = 0;
            }
            
            write_pos_ += ret;
            return 2;//没写完
        }
        
        //到这里说明包头已经写完
        write_pos_ = 0;
        data_one_ = true;
    }

    //向slave 写包体
    if (data_two_ == false) {
        char *buf;
        int len = cli->body_len_ - write_pos_;
        if(cli->body_len_ <= ONE_M) {
            buf = cli->recv_ + write_pos_;
        } else {
            buf = cli->big_recv_ + write_pos_; 
        }

        ret = link_->WriteData(buf, len);
        if (ret < len) {
            if (ret == 0) {
                log_error("slave error: %s\n", strerror(errno));
                exit(0);//return -1;
            }

            if (ret < 0) {
                if (errno != EAGAIN) {
                    log_error("slave error: %s\n", strerror(errno));
                    exit(0);//return -1;
                }

                ret = 0;
            }

            write_pos_ += ret;
            return 2; //没写完
        }
        
        //到这里说明包体已经写完
        write_pos_ = 0;
        data_two_ = true;
    }
   
    //向slave 写client的标示,这里用client 的fd 作为唯一标识
    if (flag_ == false) {
        short fd = htons(cli->link_->fd());
        memcpy(client_flag_, (char *)&fd, sizeof(short));
        flag_ = true;
    }
    
    char *buf = client_flag_ + write_pos_;
    int len = sizeof(short) - write_pos_;
    ret = link_->WriteData(buf, len);
    if (ret < len) {
        if (ret == 0) {
            log_error("slave error: %s\n", strerror(errno));
            exit(0);//return -1;
        }

        if (ret < 0) {
            if (errno != EAGAIN) {
                log_error("slave error: %s\n", strerror(errno));
                exit(0);//return -1;
            }

            ret = 0;
        }

        write_pos_ += ret;

        return 2;//没写完
    }

    //到这里说明client 的标示已经写完
    write_pos_ = 0;
    data_one_ = false;
    data_two_ = false;
    flag_ = false;

    clients_.erase(it);
    
    return 0;
}

struct event* Slave::time_event() const 
{
    return time_event_;
}

void Slave::set_time_event(struct event *e)
{
    time_event_ = e;
}
