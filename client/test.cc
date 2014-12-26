// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string>

#include "ldbc.h"

int main()
{
    ClientImpl cli;
    cli.socket_.Connect("127.0.0.1", 8899);
    
    std::string key = "key";
    std::string val = "val";
    
    cli.Set(key, val);
}
