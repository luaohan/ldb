// ldbc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "ldbc.h"

int ClientImpl::Set(std::string &key, std::string &val)
{
    const char *s_key = key.c_str();
    const char *s_val = val.c_str();
   
    /*set key val*/
    int len = strlen("set") + 1 + key.size() + 1 + val.size();

    char buffer[len];
    memcpy(buffer, "set ", 3 + 1);
    memcpy(buffer + 4, s_key, key.size());
    memcpy(buffer + 4 + key.size(), " ", 1);
    memcpy(buffer + 4 + key.size() + 1, s_val, val.size());

    socket_.writeData(buffer, len);

    char replay[1024];
    int ret = socket_.readData(replay, 1024);
    replay[ret] = '\0';
    
    printf("replay:%s\n", replay);

    return 0;
}

int ClientImpl::Get(std::string &key, std::string *val)
{

}
