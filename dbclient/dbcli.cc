// ldbc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbcli.h"

namespace ldb {
namespace dbcli {

int Client::Set(const std::string &key, const std::string &val)
{
    const char *s_key = key.c_str();
    const char *s_val = val.c_str();
   
    /*set key val*/
    int len = strlen("set") + 1 + key.size() + 1 + val.size();

    char buffer[len + 1];
    memcpy(buffer, "set ", 3 + 1);
    memcpy(buffer + 4, s_key, key.size());
    memcpy(buffer + 4 + key.size(), " ", 1);
    memcpy(buffer + 4 + key.size() + 1, s_val, val.size());
    buffer[len] = '\0';

    int ret;
    ret = socket_.WriteData(buffer, len + 1);
    if (ret < 0) {
        //log()
        return -1;
    }

    char replay[1024];
    ret = socket_.ReadData(replay, 1024);
    if (ret < 0) {
        //log()
        return -1;
    }

    replay[ret] = '\0';

    return 0;
}

int Client::Get(const std::string &key, std::string *val)
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

int Client::Del(const std::string &key)
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

    printf(">>>:%s\n", replay);

    return 0;
}

} /*dbcli*/
} /*ldb*/


