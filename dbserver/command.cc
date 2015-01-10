// command.cc (2015-01-09)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <assert.h>
#include <leveldb/slice.h>
#include <errno.h>

#include "command.h"
#include "server.h"
#include "client.h"
#include "../util/protocol.h"
#include "../util/log.h"

int SetCommand(Server *s, Client *c) 
{
    assert(s != NULL && c != NULL);

    leveldb::Slice key(c->key_, c->key_len_);

    int value_len = c->body_len_ - c->key_len_ - sizeof(short);
    leveldb::Slice val(c->val_, value_len);

    int ret = s->Insert(key, val);
    if (ret == -1) {
        //fatal error
        log_fatal("insert error");
        return -1;
    }   

    ret = FillPacket(c->replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_OK);
    int write_len = c->link_->WriteData(c->replay_, ret);
    if (write_len < ret) {
        if (errno != EAGAIN) {
            //error
            log_error("write to client error, fd[%d], port[%d], ip[%s]",
                    c->link_->GetFd(), c->link_->GetPort(), c->link_->GetIp());

            return -1;
        }   
    }   

    return 0;
}

int GetCommand(Server *s, Client *c)
{
    assert(s != NULL && c != NULL);

    leveldb::Slice key(c->key_, c->key_len_);

    std::string val;
    int ret = s->Get(key, &val);
    if (ret < 0) {
        //key not exist 
        ret = FillPacket(c->replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_NO_THE_KEY);
    } else {
        //key exist
        ret = FillPacket(c->replay_, MAX_PACKET_LEN, val.c_str(), val.size(), NULL, 0, REPLAY_OK);
    }

    int write_len = c->link_->WriteData(c->replay_, ret);
    if (write_len < ret) {
        if (errno != EAGAIN) {
            //error
            log_error("write to client error, fd[%d], port[%d], ip[%s]",
                    c->link_->GetFd(), c->link_->GetPort(), c->link_->GetIp());

            return -1;
        }   
    }   

    return 0;
}

int DelCommand(Server *s, Client *c)
{
    assert(s != NULL && c != NULL);

    leveldb::Slice key(c->key_, c->key_len_);
    s->Delete(key);

    int ret = FillPacket(c->replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_OK);
    
    int write_len = c->link_->WriteData(c->replay_, ret);
    if (write_len < ret) {
        if (errno != EAGAIN) {
            //error
            log_error("write to client error, fd[%d], port[%d], ip[%s]",
                    c->link_->GetFd(), c->link_->GetPort(), c->link_->GetIp());

            return -1;
        }   
    }   

    return 0;
}

