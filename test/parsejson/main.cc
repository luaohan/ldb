// main.cc (2015-01-15)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <vector>

#include "parsejson.h"
#include "../../net/socket.h"

unsigned int DJBHash(const unsigned char *buf, int len); 
Socket *GetSocket(const char *key, std::vector<Socket *> &server);

int main()
{
   
    std::vector<Socket *> server;

    int ret = GetNodeInfo(server, "config.json");
    if (ret < 0) {
        printf("error: %s, %d\n", strerror(errno), errno);
        return -1;
    }

    Socket *s = GetSocket("1", server);
    printf("server:%s\n", s->GetIp());
    
    s = GetSocket("2", server);
    printf("server:%s\n", s->GetIp());
    
    s = GetSocket("3", server);
    printf("server:%s\n", s->GetIp());
    
    s = GetSocket("4", server);
    printf("server:%s\n", s->GetIp());
    
    s = GetSocket("5", server);
    printf("server:%s\n", s->GetIp());

    s = GetSocket("6", server);
    printf("server:%s\n", s->GetIp());
    
    s = GetSocket("7", server);
    printf("server:%s\n", s->GetIp());
    
    s = GetSocket("8", server);
    printf("server:%s\n", s->GetIp());
   
    s = GetSocket("9", server);
    printf("server:%s\n", s->GetIp());
    
    return 0;
}

Socket *GetSocket(const char *key, std::vector<Socket *> &server)
{
    int virtual_node = 
        DJBHash((const unsigned char *)key, strlen(key)) % server.size();

    Socket *s = server[virtual_node];
    
    return s;
}


unsigned int DJBHash(const unsigned char *buf, int len) {
    static const int hash_function_seed = 5381; 
    unsigned int hash = (unsigned int)hash_function_seed;
    while (len--) {
        hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
    }

    return hash;
}
