// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string>
#include <string.h>
#include <errno.h>

#include "ldbc.h"

int main()
{
    Client cli;

    if (cli.socket_.Connect("127.0.0.1", 8899) == -1) {
        fprintf(stderr, "connect errno: %s\n", strerror(errno));
        return -1;
    }

    std::string key = "keyaaabbb";
    std::string val = "valaaabbb";
#if 0    
    std::string val2 = "valaaabbb2";
    std::string val3 = "valaaabbb3";

    cli.Set(key, val);
    cli.Get(key, &val);
    printf(":%s\n", val.c_str());
    
    cli.Del(key);
    cli.Set(key, val2);
    cli.Get(key, &val);
    printf(":%s\n", val.c_str());
   
    cli.Del(key);
    cli.Set(key, val3);
    cli.Get(key, &val);
    printf(":%s\n", val.c_str());
    
    cli.Del(key);
    cli.Del(key);
    cli.Get(key, &val);
    printf(":%s\n", val.c_str());
#endif
#if 0    
    std::string key = "keykey";
    std::string val;
    cli.Get(key, &val);
    printf(":%s\n", val.c_str());
#endif
    for (int i = 0; i < 100000; i++) {
        cli.Set(key, val);
        printf("%d\n", i);
    }

    return 0;
}
