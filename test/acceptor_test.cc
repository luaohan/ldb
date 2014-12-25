// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)
//
#include <stdio.h>
#include "../acceptor.h"

int main()
{
    Acceptor server(true);
    server.Listen("0.0.0.0", 8899, 5);

    Acceptor *client = server.Accept();

    printf("client: fd = %d, port = %d, ip = %s", client->getFd(), client->getPort(), client->getIp());

    return 0;
}
