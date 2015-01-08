// proc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "proc.h"
#include "string_type.h"
#include "../util/str.h" 
#include "../util/log.h"
#include "../net/socket.h"
#include "../net/acceptor.h"
#include "../util/protocol.h"


void process_events(Server &server)
{
    int n = server.event_.WaitReadEvent(server.fired_fd);
    if ( n < 0) {
        log_error("WaitReadEvent error:[%s]", strerror(errno));
        return ;
    }

    for (int i = 0; i < n; i++) 
    {
        if ( server.fired_fd[i] == server.socket_->GetFd()) {
            Socket *link = server.socket_->Accept();
            if (link == NULL) {
                log_error("Accept error:[%s]", strerror(errno));
                continue;
            }

            link->SetNoblock();

            server.event_.AddReadEvent(link->GetFd());

            Client *cli = new Client(link);
            server.AddClient(cli);

            log_info("---<create a client:[ip:%s],[port:%d],[fd:%d]>---", 
                    link->GetIp(), link->GetPort(), link->GetFd());
            continue;
        }

        Client *cli = server.FindClient(server.fired_fd[i]);

        int ret;
        if (cli->data_one_ == false) {
            //读包头
            ret = cli->link_->ReadData(cli->recv_ + cli->data_pos_, 
                    HEAD_LEN - cli->data_pos_);

            if (ret < HEAD_LEN - cli->data_pos_) {
                if (errno != EAGAIN || ret == 0) {
                    log_error("readData error or client exit:[%s], fd:[%d]", 
                            strerror(errno), cli->link_->GetFd());

                    server.event_.DelReadEvent(cli->link_->GetFd());
                    server.DeleteClient(cli->link_->GetFd());
                    delete cli;

                    continue;
                }

                cli->data_pos_ += ret;
                continue;
            }
            
            //到这里说明需要的东西已经读够
            //解析读到的内容
            int packet_len = ntohl(*((int *)&(cli->recv_[0])));
            short packet_type = ntohs(*((short *)&(cli->recv_[sizeof(int)])));
            if (packet_type == SET_CMD) {
                cli->cmd = ldb_set_command;

            } else if (packet_type == GET_CMD) {
                cli->cmd = ldb_get_command;
                
            } else if (packet_type == DEL_CMD) {
                cli->cmd = ldb_del_command;

            } else {
                //error cmd
                //关闭这个cli
                continue;
            }
            
            cli->body_len_ = packet_len - HEAD_LEN;

            cli->data_one_ = true;
            cli->data_pos_ = 0;
        }

        //读包体
        ret = cli->link_->ReadData(cli->recv_ + cli->data_pos_, 
                cli->body_len_ - cli->data_pos_);

        if (ret < cli->body_len_ - cli->data_pos_) {
            if (errno != EAGAIN || ret == 0) {
                log_error("readData error or client exit:[%s], fd:[%d]", 
                        strerror(errno), cli->link_->GetFd());

                server.event_.DelReadEvent(cli->link_->GetFd());
                server.DeleteClient(cli->link_->GetFd());
                delete cli;

                continue;
            }

            cli->data_pos_ += ret;
            continue;
        }

        //到这里说明需要的东西已经读够
        //解析读到的内容, 放到cli 的响应成员
        short key_len = ntohs(*((short *)&(cli->recv_[0])));
        cli->key_len_ = key_len;
        memcpy(cli->key_, cli->recv_ + sizeof(short), key_len);
        memcpy(cli->val_, cli->recv_ + sizeof(short) + key_len, 
                cli->body_len_ - key_len - sizeof(short));

        cli->data_one_ = false;
        cli->data_pos_ = 0;
        
        cli->cmd(&server, cli);

    }

    return ;
}
