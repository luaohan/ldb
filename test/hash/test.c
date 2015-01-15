#include <stdio.h>
#include <string.h>
#include <ctype.h>

//virtual node count as array size, get from config
//virtual node number as array index, get from config, must be series
//server ip as array element
//such as:
//array[0] -- 192.168.0.1
//array[1] -- 192.168.0.1
//array[3] -- 192.168.0.1
//array[4] -- 192.168.0.2
//array[5] -- 192.168.0.2
//...

//sample
//virtual node => server ip
#define VIRUTAL_NODE_COUNT 9
char g_server[VIRUTAL_NODE_COUNT][16] = {
    "192.168.0.1", /*virtual node  0, array index 0*/
    "192.168.0.1", /*virtual node  1, array index 1*/
    "192.168.0.1", /*virtual node  2*/
    "192.168.0.2", /*virtual node  3*/
    "192.168.0.2", /*virtual node  4*/
    "192.168.0.2", /*virtual node  5*/
    "192.168.0.3", /*virtual node  6*/
    "192.168.0.3", /*virtual node  7*/
    "192.168.0.3"  /*virtual node  8*/
};

unsigned int DJBHash(const unsigned char *buf, int len) {
    static const int hash_function_seed = 5381;
    unsigned int hash = (unsigned int)hash_function_seed;
    while (len--) {
        hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
    }

    return hash;
}

void GetServer(const char *key)
{
   int virtual_node = DJBHash((const unsigned char *)key, strlen(key)) % VIRUTAL_NODE_COUNT;
   const char *server = g_server[virtual_node];

   //for test
   printf("key: %s, virutal node: %d, server: %s\r\n", key, virtual_node, server);
}

int main()
{
    GetServer("aa");
    GetServer("bb");
    GetServer("cc");
    GetServer("dd");
    GetServer("ee");

   return 0;
}
