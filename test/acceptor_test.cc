// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)
//
#include <stdio.h>
#include "../acceptor.h"

int main()
{
    ldb::event::Loop loop;
    Acceptor server(&loop);
    server.Listen("0.0.0.0", 8899, 5);

    loop.Run();

    return 0;
}
