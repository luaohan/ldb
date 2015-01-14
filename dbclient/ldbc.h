// ldbc.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _LDBC_H_
#define _LDBC_H_

#include <dbclient/status.h>

class Client {
public:
    Client();
    ~Client();

    bool Connect(const std::string &ip, int port);
    void Close();

    Status Set(const std::string &key, const std::string &value);
    Status Del(const std::string &key);
    Status Get(const std::string &key, std::string *value); 

private:
    Client(const Client &c);
    Client & operator = (const Client &c);

private:
    class Impl;
    Impl *impl_;
};

#endif
