// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)
//
#include "stdio.h"
#include "socket.h"

int main()
{
    Socket server(true);
    server.Listen("0.0.0.0", 8899, 5);

    Socket *client = server.Accept();

    printf("client: fd = %d, port = %d, ip = %s", client->getFd(), client->getPort(), client->getIp());

    return 0;
}
