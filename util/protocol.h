// protocol.h (2015-01-08)
// WangPeng (1245268612@qq.com)

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_


#define SET_CMD 0
#define GET_CMD 1
#define DEL_CMD 2

#define REPLAY_OK 3

/*
 *|unsigned int   |unsigned short|unsigned short|
 *|packet_len     |type          |key_len       | key | value |
 * */
#define MAX_KET_LEN 65535 //64k
#define MAX_VAL_LEN 65535 //64k

#define HEAD_LEN (sizeof(unsigned int) + sizeof(unsigned short))

#define MAX_PACKET_LEN sizeof(unsigned int) + sizeof(unsigned short) + \
    sizeof(unsigned short) + 65535 + 65535 

//任何时候函数返回整个包的长度
//调用者需要自己判断返回值是否大于buf_len
//如果返回值大于buf_len, 说明数据包没有构造成功，
//调用者需要重新寻找跟大的buf, 来构造数据包,
//允许key 和 value 同时 为 NULL，这时只有包头
int FillPacket(char *buf, int buf_len, char *key, int key_len, 
        char *value, int value_len, short data_type);











#endif
