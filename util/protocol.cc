// protocol.cc (2015-01-08)
// WangPeng (1245268612@qq.com)


#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "protocol.h"

int FillPacket(char *buf, int buf_len, char *key, int key_len, 
        char *value, int value_len, short data_type)
{
    assert(buf != NULL);
    assert(buf_len > 0 /*&& key_len > 0 && value_len > 0*/);
    //assert(key != NULL && value != NULL);

    int valid_packet_len = sizeof(unsigned int) + sizeof(unsigned short) + 
        sizeof(unsigned short) + key_len  + value_len;

    if (valid_packet_len > buf_len) {
        return valid_packet_len ;
    }

    int packet_len = htonl(valid_packet_len);
    short packet_type = htons(data_type);

    char *p = buf;
    memcpy(p, (char *)&packet_len, sizeof(unsigned int));
    p += sizeof(unsigned int);
    memcpy(p, (char *)&packet_type, sizeof(unsigned short));
    p += sizeof(unsigned short);

    if (key == NULL || value == NULL) {
        return HEAD_LEN; 
    }
    
    short key_len_net = htons(key_len);
    //short val_len_net = htons(value_len);
    // key
    memcpy(p, (char *)&key_len_net, sizeof(unsigned short));
    p += sizeof(unsigned short);
    memcpy(p, key, key_len);
    p += key_len;

    //value
    //memcpy(p, (char *)&val_len_net, sizeof(short));
    //p += sizeof(short);
    memcpy(p, value, value_len);
    
    return valid_packet_len;
}
