// ldbc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "ldbc.h"
#include "../util/protocol.h"

int Client::Set(std::string &key, std::string &val)
{
    const char *s_key = key.c_str();
    const char *s_val = val.c_str();
  
#if 0
    /*set key val*/
    int len = strlen("set") + 1 + key.size() + 1 + val.size();

    char buffer[len + 1];
    memcpy(buffer, "set ", 3 + 1);
    memcpy(buffer + 4, s_key, key.size());
    memcpy(buffer + 4 + key.size(), " ", 1);
    memcpy(buffer + 4 + key.size() + 1, s_val, val.size());
    buffer[len] = '\0';
#endif

    char buf[BUFSIZ]; //1024 * 8 = 8k
                      //经过测试本系统的socket  默认的缓冲区的大小是 32768 字节，32k
    int len = fill_packet(buf, BUFSIZ, s_key, key.size(), 
            s_val, val.size(), SET_CMD);
    if (len > BUFSIZ) { //说明一个缓冲区的长度无法容纳下整个包
                        //这时需要在堆上申请一个大的空间
        return -1;    //现在先返回 -1
    }

    int ret;
    ret = socket_.WriteData(buffer, len);
    if (ret < 0) {
        //log()
        if (errno == EAGAIN) {
            return -2; //下一次应该由写事件来驱动
        }
       
        //error
        return -1;
    }

    char replay[1024];
    ret = socket_.ReadData(replay, 1024); //读时应该来解析协议
    if (ret < 0) {
        //log()
        return -1;
    }

    replay[ret] = '\0';

    return 0;
}

int Client::Get(std::string &key, std::string *val)
{
    const char *s_key = key.c_str();
    
    /*get key*/
    int len = strlen("get") + 1 + key.size();
    char buffer[len + 1];
    memcpy(buffer, "get ", 3 + 1);
    memcpy(buffer + 4, s_key, key.size());
    buffer[len] = '\0';

    int ret;
    ret = socket_.WriteData(buffer, len + 1);
    if (ret < 0) {
        //log();
        return -1;
    }

    char replay[1024];
    ret = socket_.ReadData(replay, 1024);
    if (ret < 0) {
        //log()
        return -1;
    }
    replay[ret] = '\0';

    buffer_.assign(replay, ret + 1);

    *val = buffer_;

    return 0;
}

int Client::Del(std::string &key)
{
    const char *s_key = key.c_str();
    
    /*get key*/
    int len = strlen("del") + 1 + key.size();
    char buffer[len + 1];
    memcpy(buffer, "del ", 3 + 1);
    memcpy(buffer + 4, s_key, key.size());
    buffer[len] = '\0';

    int ret;
    ret = socket_.WriteData(buffer, len + 1);
    if (ret < 0) {
        //log();
        return -1;
    }

    char replay[1024];
    ret = socket_.ReadData(replay, 1024);
    if (ret < 0) {
        return -1;
    }

    replay[ret] = '\0';

    return 0;
}
