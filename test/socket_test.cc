// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)
//
#include <stdio.h>
#include "../client/socket.h"

int main()
{
    ldb::event::Loop loop;
    Socket c(&loop);
    c.Connect("127.0.0.1", 8899);
    bool ret = loop.Run();
    if (!ret) {
        //LOG(ERROR);
        return -1;
    }

    return 0;
}
