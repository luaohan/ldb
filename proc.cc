// proc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "proc.h"
#include "str.h" 
#include "string_type.h"

extern Server server;

int ldb_write(int fd, void *buf, int len)   
{
    int write_len;

write_again:
    write_len = write(fd, buf, len);
    if (write_len < 0)  
    {   
        if (write_len == EINTR) {
            goto write_again;
        }   

        return write_len;
    }   

    return write_len;
}

int ldb_tell_client( Client *client )
{
    return ldb_write(client->fd_, client->replay_, strlen(client->replay_));
}

void process_client_info(Client *client) 
{
    fprintf(stderr, "client_fd:%d, data:|%s|\n", client->fd_, client->recv_);

    str2lower(client->recv_);
    strs2tokens(client->recv_, LDB_SPACE, client->argv_, &client->argc_);

    int tell_len;
    Command *command = server.FindCommand(client->argv_[0]);
    if (command == NULL) {
        memcpy(client->replay_, LDB_NO_THE_COMMAND, strlen(LDB_NO_THE_COMMAND));

        tell_len = ldb_tell_client(client);
        if (tell_len < 0) {
            memcpy(client->replay_, 0, 2048);
            fprintf(stderr, "ldb_tell_client error:%s\n", strerror(errno));
            return ;
        }

        memset(client->recv_, 0, 2048);
        memset(client->replay_, 0, 2048);
        return ;
    }
    
    client->cmd = command;
    command->proc(&server, client);
    tell_len = ldb_tell_client(client);
    if (tell_len < 0) {
        fprintf(stderr, "ldb_tell_client error:%s\n", strerror(errno));
        return ;
    }
   
    memset(client->recv_, 0, 2048);
    memset(client->replay_, 0, 2048);
    return ;
}
