#include <stdio.h>
#include <ctype.h>
static int hash_function_seed = 5381;
unsigned int HashFunction(const unsigned char *buf, int len);
int select_server(int hash_value);
int server1[9] = {1, 1, 1, -1, -1, -1, -1, -1, -1};
int server2[9] = {-1, -1, -1, 1, 1, 1, -1, -1, -1};
int server3[9] = {-1, -1, -1, -1, -1, -1, 1, 1, 1};
int main()
{
   char *key1 = "a";

   int hash1 = HashFunction(key1, 1) % 9;
    
   int server = select_server(hash1);
    
   return 0;
}
int select_server(int hash_value) {
    if (server1[hash_value] != -1) {
        return 1;
    } else if (server2[hash_value] != -1) {
        return 2;
    } else if (server3[hash_value] != -1) {
        return 3;
    }

    return -1;
}
unsigned int HashFunction(const unsigned char *buf, int len) {
    unsigned int hash = (unsigned int)hash_function_seed;
    while (len--)
        hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
    return hash;
}
