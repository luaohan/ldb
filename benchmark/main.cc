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

using namespace std;

int main(int argc, char *argv[])
{
    std::string conf_file("config.json");
    Client cli(conf_file); 

    int ret = cli.Init();
    if (ret != 0) {
        fprintf(stderr, "error: %s, ret: %d\n", strerror(errno), ret);
        return -1;
    }

    Test t(&cli);
    t.Run();

    return 0;
}
