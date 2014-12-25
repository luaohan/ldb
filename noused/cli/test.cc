// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)
//
#include "stdio.h"
#include "socket.h"

int main()
{
    Socket server(true);
    server.Connect("127.0.0.1", 8899);

    printf("client: fd = %d, port = %d, ip = %s", server.getFd(), server.getPort(), server.getIp());

    return 0;
}
