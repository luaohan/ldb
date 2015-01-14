// command.cc (2015-01-09)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <leveldb/slice.h>
#include <errno.h>

#include "command.h"
#include "server.h"
#include "client.h"
#include "../util/protocol.h"
#include "../util/log.h"

#if 0

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
    c->replay_len_ = ret;
    ret = c->WritePacket();

    if (ret == 2) { // 没写完
        //将写事件加入epoll
        Event e;
        e.fd_ = c->link_->GetFd();
        e.ptr_ = c;
        
        s->event_.AddWriteEvent(e);
    } else if (ret == -1) { //error
        return -1;
    }

    //到这里说明已经写完
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
   
    c->replay_len_ = ret;
    ret = c->WritePacket();

    if (ret == 2) { // 没写完

        //将写事件加入epoll
        Event e;
        e.fd_ = c->link_->GetFd();
        e.ptr_ = c;
        
        s->event_.AddWriteEvent(e);
    } else if (ret == -1) { //error
        return -1;
    }

    //到这里说明已经写完
    return 0;
}

#endif

int SetCommand(Server *s, Client *c) 
{
    assert(s != NULL && c != NULL);

    leveldb::Slice key(c->key_, c->key_len_);

    int ret;
    int value_len = c->body_len_ - c->key_len_ - sizeof(short);

    if (c->big_value_ == NULL) {

        leveldb::Slice val(c->val_, value_len);
        ret = s->Insert(key, val);
        if (ret == -1) {
            //fatal error
            log_fatal("insert error");
            return -1;
        }   
    } else {
        leveldb::Slice val(c->big_value_, value_len);
        ret = s->Insert(key, val);
        if (ret == -1) {
            //fatal error
            log_fatal("insert error");
            return -1;
        }   
        free(c->big_value_);
        c->big_value_ = NULL;
    }

    ret = FillPacket(c->replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_OK);
    c->replay_len_ = ret;
    ret = c->WritePacket();
    
    if (ret == 2) { // 没写完
        //将写事件加入epoll
        Event e;
        e.fd_ = c->link_->GetFd();
        e.ptr_ = c;
        
        s->event_.AddWriteEvent(e);
    } else if (ret == -1) { //error
        return -1;
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
        //如果value 大于ONE_M, 需要申请内存
        if (val.size() <= ONE_M) {

            ret = FillPacket(c->replay_, MAX_PACKET_LEN, val.c_str(), val.size(), 
                    NULL, 0, REPLAY_OK);
    
        } else {
            int size = val.size() + sizeof(int) + sizeof(short) * 2;
            if (c->big_value_ == NULL) {
                c->big_value_ = (char *)malloc(size);
                if (c->big_value_ == NULL) {
                    return -1;
                }
            }

            ret = FillPacket(c->big_value_, size, NULL, val.size(), 
                    NULL, 0, REPLAY_OK);
            memcpy(c->big_value_ + sizeof(int) + sizeof(short) * 2, 
                    val.c_str(), val.size());
        
        }
    }

    c->replay_len_ = ret;
    ret = c->WritePacket();

    if (ret == 2) { // 没写完

        //将写事件加入epoll
        Event e;
        e.fd_ = c->link_->GetFd();
        e.ptr_ = c;
        
        s->event_.AddWriteEvent(e);
    } else if (ret == -1) { //error
        return -1;
    }

    return 0;
}

int DelCommand(Server *s, Client *c)
{
    assert(s != NULL && c != NULL);

    leveldb::Slice key(c->key_, c->key_len_);
    s->Delete(key);

    int ret = FillPacket(c->replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_OK);
    c->replay_len_ = ret;
    ret = c->WritePacket();

    if (ret == 2) { // 没写完
        //将写事件加入epoll
        Event e;
        e.fd_ = c->link_->GetFd();
        e.ptr_ = c;
        
        s->event_.AddWriteEvent(e);
    } else if (ret == -1) { //error
        return -1;
    }
    
    return 0;
}
