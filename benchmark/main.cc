// main.cc (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <iostream>
#include <dbclient/client.h>
#include "test.h"
#include "benchmark.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("<%s> <configfile>\n", argv[0]);
        return -1;
    }
    
    std::string conf_file(argv[1]);
    Client cli(conf_file); 

    int ret = cli.Init();
    if (ret != 0) {
        fprintf(stderr, "error: %s, ret: %d\n", strerror(errno), ret);
        return -1;
    }
    
    printf("Init Server Ok\n");

#if 0
    Test t(&cli);
    t.Run();
#endif

    Benchmark bench(&cli);
    bench.Run();
    
    return 0;
}
