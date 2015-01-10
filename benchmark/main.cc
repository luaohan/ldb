// main.cc (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

#include <string.h>
#include <errno.h>

#include <iostream>
#include <dbclient/ldbc.h>
#include "test.h"

using namespace std;

int main(int argc, char *argv[])
{
    Client cli;

    if (cli.socket_.Connect("127.0.0.1", 8899) == -1) {
        cout << "connect error: " << strerror(errno) << endl;
        return -1;
    }


    Test t(&cli);
    t.Run();

    return 0;
}
