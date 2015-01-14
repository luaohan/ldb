// protocol.h (2015-01-08)
// WangPeng (1245268612@qq.com)

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_


#define SET_CMD 0
#define GET_CMD 1
#define DEL_CMD 2

#define REPLAY_OK 3

#define REPLAY_NO_THE_KEY 4

#define MAX_KEY_LEN 65535 //64k
#define MAX_VAL_LEN 10485760    //10M
#define ONE_M       1048576    

#define HEAD_LEN (sizeof(int) + sizeof(short))

#define MAX_PACKET_LEN sizeof(int) + sizeof(short) + \
    sizeof(int/*short*/) + MAX_KEY_LEN + MAX_VAL_LEN 



//|packet_len | type |                       |    or
//|pakcet_len | type | key_len | key |       |    or
//|packet_len | type | key_len | key | value |    
int FillPacket(char *buf, int buf_len, const char *key, int key_len,
        const char *value, int value_len, short data_type);


#endif
