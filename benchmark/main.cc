// main.cc (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <dbclient/client.h>
#include "test.h"

using namespace std;

int main(int argc, char *argv[])
{
    Client cli(false); //非分布式
    if (cli.Connect("127.0.0.1", 8899) == false) {
        cout << "connect error: " << strerror(errno) << ", errno: " << errno << endl;
        return -1;
    }
    

#if 0
    Client cli(true); //分布式
    std::string file_name = "config.json";

    int ret = cli.Init(file_name);
    if (ret == -1) {
        fprintf(stderr, "connect errno: %s\n", strerror(errno));
        return -1; 
    } else if (ret == -2) {
        fprintf(stderr, "configfile error\n");
        return -1; 
    }   
    
    printf("init ok\n");
#endif

    Test t(&cli);
    t.Run();

    return 0;
}
