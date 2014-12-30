// ldbc.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _LDBC_H_
#define _LDBC_H_

#include <string>
#include <net/socket.h>

namespace ldb {
namespace dbcli {

class Client {
public:
    Client(){ };
    ~Client(){ };

    //ok: return 0
    //error: return -1
    int Set(const std::string &key, const std::string &value);
    int Get(const std::string &key, std::string *value);
    int Del(const std::string &key);

public:
    ldb::net::Socket socket_;
    std::string buffer_;
};

} /*namespace ldb*/
} /*namespace dbcli*/

#endif

