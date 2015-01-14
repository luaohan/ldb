// ldbc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "ldbc.h"
#include "../util/protocol.h"

int Client::Set(std::string &key, std::string &val)
{
    const char *s_key = key.c_str();
    const char *s_val = val.c_str();

    char buf[MAX_PACKET_LEN]; 
    
    if (val.size() <= ONE_M) {

        int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(),
                s_val, val.size(), SET_CMD);
        
        int ret = socket_.BlockWrite(buf, len);
        if (ret < 0) {
            return -1;
        }

        ret = socket_.BlockRead(buf, HEAD_LEN);
        if (ret < 0) {
            return -1;
        }
        if (ret > 0 && ret < HEAD_LEN) {
            return -2; //server exit
        }
        
        return 0;
    }
    
    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), 
            NULL, val.size(), SET_CMD);
    
    int ret = socket_.BlockWrite(buf, len - val.size());
    if (ret < 0) {
        return -1;
    }
    
    ret = socket_.BlockWrite((char *)s_val, val.size());
    if (ret < 0) {
        return -1;
    }
    
    ret = socket_.BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        return -1;
    }
    if (ret > 0 && ret < HEAD_LEN) {
        return -2; //server exit
    }

    return 0;

}

int Client::Get(std::string &key, std::string *val)
{
    const char *s_key = key.c_str();
    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, GET_CMD);
    int ret = socket_.BlockWrite(buf, len);
    if (ret < 0) {
        return -1;
    }

    ret = socket_.BlockRead(buf, HEAD_LEN); //读包头
    if (ret < 0) {
        return -1;
    }
    if (ret > 0 && ret < HEAD_LEN) {
        return -2; //server exit
    }

    int packet_len = ntohl(*((int *)&(buf[0])));
    short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
    if (packet_type == REPLAY_NO_THE_KEY) {
        return REPLAY_NO_THE_KEY;
    }

    int body_len = packet_len - HEAD_LEN;

    if (body_len <= ONE_M) {
        
        ret = socket_.BlockRead(buf, body_len); //读包体
        if (ret < 0) {
            return -1;
        }
        if (ret > 0 && ret < HEAD_LEN) {
            return -2; //server exit
        }

        //short value_len = ntohs(*((short *)&(buf[0])));
        int value_len = body_len - sizeof(short);

        std::string value(&buf[sizeof(short)], value_len);
        *val = value;

        return 0;
    }

    char *p = (char *)malloc(body_len);
    if (p == NULL) {
        return -1;
    }

    ret = socket_.BlockRead(p, body_len); //读包体
    if (ret < 0) {
        free(p);
        return -1;
    }
    if (ret > 0 && ret < HEAD_LEN) {
        free(p);
        return -2; //server exit
    }

    //short value_len = ntohs(*((short *)&(buf[0])));
    int value_len = body_len - sizeof(short);

    std::string value(&p[sizeof(short)], value_len);
    *val = value;

    free(p);

    return 0;
}

int Client::Del(std::string &key)
{
    const char *s_key = key.c_str();

    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, DEL_CMD);
    int ret = socket_.WriteData(buf, len);
    if (ret < 0) {
        return -1;
    }

    ret = socket_.BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        return -1;
    }
    if (ret > 0 && ret < HEAD_LEN) {
        return -2; //server exit
    }

    return 0;
}
