// proc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "proc.h"
#include "../util/str.h" 
#include "../util/log.h"
#include "../net/socket.h"
#include "../net/acceptor.h"
#include "../util/protocol.h"

int tell_client( Client *client )
{
    return client->link_->WriteData(client->replay_, strlen(client->replay_));
}

void process_client_info(Server &server, Client *client) 
{
    log_info("ip:[%s], port:[%d], data:[%s]", client->link_->GetIp(),
            client->link_->GetPort(),client->recv_);

    str2lower(client->recv_);
    strs2tokens(client->recv_, LDB_SPACE, client->argv_, &client->argc_);

    int tell_len;
    Command *command = server.FindCommand(client->argv_[0]);
    if (command == NULL) {
        memcpy(client->replay_, LDB_NO_THE_COMMAND, strlen(LDB_NO_THE_COMMAND) + 1);

        tell_len = tell_client(client);
        if (tell_len < 0) {
            log_error("ip:[%s], port:[%d], tell_client error:[%s]",
                    client->link_->GetIp(), client->link_->GetPort(),
                    strerror(errno));
            return ;
        }

        return ;
    }
    
    client->cmd = command;
    command->proc(&server, client);
    tell_len = tell_client(client);
    if (tell_len < 0) {
        log_error("ip:[%s], port:[%d], tell_client error:[%s]", 
                client->link_->GetIp(), client->link_->GetPort(), 
                strerror(errno));
        return ;
    }

    return ;
}

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
            ret = cli->link_->ReadData(cli->recv_ + cli->data_one_pos_, 
                    HEAD_LEN - cli->data_one_pos_);

            if (ret < HEAD_LEN) {
                if (errno != EAGAIN) {
                    log_error("readData error:[%s], fd:[%d]", 
                            strerror(errno), cli->link_->getFd());

                    server.event_.DelReadEvent(cli->link_->GetFd());
                    server.DeleteClient(cli->link_->GetFd());
                    delete cli;

                    continue;
                }

                cli->data_one_pos_ += ret;
            }
        }


#if 0
        int ret;
        ret = cli->link_->ReadData(cli->recv_ + cli->pos_, BUFSIZ);
        if (ret == -1) {
            if (errno != EAGAIN) {
                log_error("readData error:[%s], fd:[%d]", 
                        strerror(errno), cli->link_->getFd());

                server.event_.DelReadEvent(cli->link_->getFd());
                server.DeleteClient(cli->link_->getFd());
                delete cli;

                continue;
            }
        }

        if (ret == 0) { //a client exit
            log_info("---<a client exit, ip:[%s],port:[%d]>---",
                    cli->link_->GetIp(), cli->link_->GetPort());

            server.event_.DelReadEvent(cli->link_->GetFd());
            server.DeleteClient(cli->link_->GetFd());
            delete cli;

            continue;
        }

        cli->pos_ += ret;

        if (cli->data_one_ == false && cli->data_two_ == false && 
                cli->pos_ < HEAD_LEN) { //接收到的数据太少，暂不解析, 返回 继续接收

            continue;
        } else if (cli->data == false) {
            cli->data_one_ == true;
            int packet_len = ntohl(*((int *)&(cli->recv_[0])));
            short packet_type = ntohl(*((short *)&(cli->recv_[sizeof(int)])));

            int body_len = packet_len - packet_type;
            if (cli->pos_ - ret < body_len) { //body_len 不够，返回继续接收
                cli->pos_ = 0;
            }
        }

#endif


#if 0
        int ret;
        if (cli->data_one_ == false) {
            ret = cli->link_->readData(cli->recv_ + cli->pos_, 4 - cli->pos_);
            if (ret < 4 - cli->pos_) {
                cli->pos_ = ret;
            }

            cli->data_one_ == true;
        }

        int data_len = ntohl(*(int *)&cli->recv_[0]);
        fprintf(stderr, "data_len: %d\n", data_len);

        ret = cli->link_->readData(cli->recv_ + cli->pos_, data_len);
        if (ret < 4) {
            cli->pos_ = ret;
        }
#endif

#if 0
        int ret = cli->link_->readData(cli->recv_, 2048);
        if (ret < 0) {
            log_error("[ip:%s],[port:%d],ReadData error:%s", cli->link_->getIp(),
                    cli->link_->getPort(), strerror(errno));

            server.event_.delReadEvent(cli->link_->getFd());
            server.DeleteClient(cli->link_->getFd());
            delete cli;
            continue;
        }

        if (ret == 0) { //a client exit
            log_info("---<a client exit, ip:[%s],port:[%d]>---",
                    cli->link_->getIp(), cli->link_->getPort());

            server.event_.delReadEvent(cli->link_->getFd());
            server.DeleteClient(cli->link_->getFd());
            delete cli;
            
            continue;
        }
#endif

        process_client_info(server, cli);
    }

    return ;
}
