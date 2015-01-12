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
    sizeof(int) + MAX_KEY_LEN + MAX_VAL_LEN 


//任何时候函数返回整个包的长度
//调用者需要自己判断返回值是否大于buf_len
//如果返回值大于buf_len, 说明数据包没有构造成功，
//调用者需要重新寻找跟大的buf, 来构造数据包,
//允许key 和 value 同时 为 NULL，这时只有包头
//也允许其中任意一个为 NULL
int FillPacket(char *buf, int buf_len, const char *key, int key_len,
        const char *value, int value_len, short data_type);










#endif
