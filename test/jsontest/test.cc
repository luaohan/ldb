// test.cc (2015-01-16)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <json/json.h>
#include "../../net/socket.h"

unsigned int DJBHash(const unsigned char *buf, int len); 
Socket *GetSocket(const char *key, std::vector<Socket *> &server);  
int GetNodeInfo(std::vector<Socket *> &server, const std::string &file_name);

#define BUFSIZE 1024 * 1024 * 10 //10M, 足以容纳config 的内容

int main()
{
    std::vector<Socket *> server;
    std::string file_name = "config.json";
    
    int ret = GetNodeInfo(server, file_name);
    if (ret < 0) {
        printf("error\n");
        return -1;
    }
    
    Socket *s = GetSocket("1", server);
    printf("server:%s\n", s->ip());

    s = GetSocket("2", server);
    printf("server:%s\n", s->ip());

    s = GetSocket("3", server);
    printf("server:%s\n", s->ip());

    s = GetSocket("4", server);
    printf("server:%s\n", s->ip());

    s = GetSocket("5", server);
    printf("server:%s\n", s->ip());

    s = GetSocket("6", server);
    printf("server:%s\n", s->ip());

    s = GetSocket("7", server);
    printf("server:%s\n", s->ip());

    s = GetSocket("8", server);
    printf("server:%s\n", s->ip());

    s = GetSocket("9", server);
    printf("server:%s\n", s->ip());

    return 0;
}

//return -1, 错误
//return -2, json 文件格式错误
//return  0, 正确
int GetNodeInfo(std::vector<Socket *> &server, const std::string &file_name)
{
    int fd = open(file_name.c_str(), O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    char buf[BUFSIZE];
    int read_size = read(fd, buf, BUFSIZE);
    if (read_size < 0) {
        return -1;
    }

    buf[read_size] = '\0';

    Json::Reader reader;
    Json::Value json_object;
    if (!reader.parse(buf, json_object)) {
        return -2;
    }
    
    int node_num = json_object["node_num"].asInt();
    server.resize(node_num);
    
    std::list<int> all_nums;

    Socket *socket = NULL;
    Json::Value array = json_object["node_maps"];
    for (int i = 0; i < array.size(); i++) 
    {
        Json::Value obj = array[i];
        Json::Value::Members member = obj.getMemberNames(); 
        for(Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter) 
        { 
            std::string ip = obj[(*iter)]["ip"].asString();
            int port = obj[(*iter)]["port"].asInt();

            socket = new Socket(ip.c_str(), port);

            Json::Value num_array = obj[(*iter)]["virtual_node"];
            for (int j = 0; j < num_array.size(); j++) 
            {
                int num = num_array[j].asInt();
                if (num >= node_num) {
                    return -2;
                }
                
                server[num] = socket;

                all_nums.push_back(num);
            }
        }  

    }
   
    if (all_nums.size() != node_num) {
        return -2;
    }
    
    all_nums.sort();
   
    std::list<int>::iterator i; 
    i = all_nums.begin();
    if (*i != 0) { //是否从0 开始
        return -2;
    }
    
    int before = 0;
    i++; 
    for (; i != all_nums.end(); i++) { //是否连续
       if (*i - before != 1) {
           return -2;
       }
       before = *i;
    }
    
    close(fd);

    return 0;
}

Socket *GetSocket(const char *key, std::vector<Socket *> &server)
{
    int virtual_node =
        DJBHash((const unsigned char *)key, strlen(key)) % server.size();

    Socket *s = server[virtual_node];

    return s;
}


unsigned int DJBHash(const unsigned char *buf, int len) 
{
    static const int hash_function_seed = 5381; 
    unsigned int hash = (unsigned int)hash_function_seed;
    while (len--) {
        hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
    }

    return hash;
}
