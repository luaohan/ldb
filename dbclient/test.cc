// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string>

#include "ldbc.h"

int main()
{
    ClientImpl cli;
    //cli.socket_.setNoblock();

    cli.socket_.setNoNagle();

    cli.socket_.Connect("127.0.0.1", 8899);
    
    std::string key = "keyaaabbb";
    std::string val = "valaaabbb";
    std::string val2 = "valaaabbb2";
    std::string val3 = "valaaabbb3";
    
    //for (int i = 0; i < 100000; i++) {
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
    //    printf("%d\n", i);
    //}
    
    //for (long int i = 0; i < 1000000000; i++) {}

    return 0;
}
