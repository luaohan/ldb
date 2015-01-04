// proc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "proc.h"
#include "str.h" 
#include "../util/log.h"

#if 0
extern Log *info_log;
extern Log *error_log;
#endif

int tell_client( Client *client )
{
    return client->link_->writeData(client->replay_, strlen(client->replay_));
}

void process_client_info(Server &server, Client *client) 
{
    //fprintf(stderr, "client_fd:%d, data:|%s|\n", client->link_->getFd(), client->recv_);
    log_info("ip:[%s], port:[%d], data:[%s]", client->link_->getIp(),
            client->link_->getPort(),client->recv_);

    str2lower(client->recv_);
    strs2tokens(client->recv_, LDB_SPACE, client->argv_, &client->argc_);

    int tell_len;
    Command *command = server.FindCommand(client->argv_[0]);
    if (command == NULL) {
        memcpy(client->replay_, LDB_NO_THE_COMMAND, strlen(LDB_NO_THE_COMMAND));

        tell_len = tell_client(client);
        if (tell_len < 0) {
            //fprintf(stderr, "tell_client error:%s\n", strerror(errno));
            log_error("ip:[%s], port:[%d], tell_client error:[%s]",
                    client->link_->getIp(), client->link_->getPort(),
                    strerror(errno));
            return ;
        }

        return ;
    }
    
    client->cmd = command;
    command->proc(&server, client);
    tell_len = tell_client(client);
    if (tell_len < 0) {
        //fprintf(stderr, "tell_client error:%s\n", strerror(errno));
        log_error("ip:[%s], port:[%d], tell_client error:[%s]", 
                client->link_->getIp(), client->link_->getPort(), 
                strerror(errno));
        return ;
    }

    return ;
}

void process_events(Server &server)
{
    int n = server.event_.waitReadEvent(server.fired_fd);
    if ( n < 0) {
        log_error("WaitReadEvent error:[%s]", strerror(errno));
        return ;
    }

    for (int i = 0; i < n; i++) 
    {
        if ( server.fired_fd[i] == server.socket_.getFd()) {
            Acceptor *link = server.socket_.Accept();
            if (link == NULL) {
                log_error("Accept error:[%s]", strerror(errno));
                continue;
            }

            link->setNoblock();

            server.event_.addReadEvent(link->getFd());

            Client *cli = new Client(link);
            server.AddClient(cli);

            //fprintf(stderr, "create a client:fd:%d \n");
            log_info("---<create a client:[ip:%s],[port:%d],[fd:%d]>---", 
                    link->getIp(), link->getPort(), link->getFd());
            continue;
        }

        Client *cli = server.FindClient(server.fired_fd[i]);

        int ret = cli->link_->readData(cli->recv_, 2048);
        if (ret < 0) {
            log_error("[ip:%s],[port:%d],ReadData error:%s", cli->link_->getIp(),
                    cli->link_->getPort(), strerror(errno));

            server.event_.delReadEvent(cli->link_->getFd());
            server.DeleteClient(cli->link_->getFd());
            delete cli->link_;
            delete cli;
            continue;
        }

        if (ret == 0) { //a client exit
            log_info("---<a client exit, ip:[%s],port:[%d]>---",
                    cli->link_->getIp(), cli->link_->getPort());

            server.event_.delReadEvent(cli->link_->getFd());
            server.DeleteClient(cli->link_->getFd());
            delete cli->link_;
            delete cli;
            //fprintf(stderr, "a client exit.\n");
            continue;
        }

        process_client_info(server, cli);
    }

    return ;
}
