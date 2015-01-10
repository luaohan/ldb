// ldbc.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _LDBC_H_
#define _LDBC_H_

#include <string>

#include "../net/socket.h"

class Client {

    public:
        Client():buffer_(NULL), buffer_size_(0){ };
        ~Client(){ 
            if (buffer_ != NULL) {
                delete buffer_;
            }
        };

        //成功返回 0
        //错误返回 -1
        //返回 -2 代表server 退出
        //如果key or value 太大，返回实际所需的数据包的大小
        int Set(std::string &key, std::string &value); //可以set 两个相同的key,
                                                       //第一个将被覆盖

        //成功返回 0
        //错误返回 -1
        //返回 -2 代表server 退出
        int Del(std::string &key);                     //可以删除一个不存在的key

        //成功返回 0, 值保存在value 中
        //错误返回 -1
        //返回 -2 代表server 退出
        //key 不存在时，返回 4
        int Get(std::string &key, std::string *value); 
                                                       

    public:
        Socket socket_;
        char *buffer_;
        int buffer_size_;

};


#endif
