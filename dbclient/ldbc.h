// ldbc.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _LDBC_H_
#define _LDBC_H_

#include <string>

#include "../net/socket.h"

class Client {

    public:
        Client(){ };
        ~Client(){ };

        //以下三个方法都是，成功则返回0，
        //错误返回-1
        int Set(std::string &key, std::string &value); //可以set 两个相同的key,
                                                       //第一个将被覆盖

        int Del(std::string &key);                     //可以删除一个不存在的key

        int Get(std::string &key, std::string *value); //获取key 的值，
                                                       //值保存在value 中,
                                                       //如果数据库中存在key
                                                       //value 将返回key 的真实值
                                                       //若不存在, value 返回
                                                       //的值将是 "no the key"

    public:
        Socket socket_;
        std::string buffer_;

};


#endif
