// ldbc.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _LDBC_H_
#define _LDBC_H_

#include <string>

#include "socket.h"

class ClientImpl {

    public:
        ClientImpl(){ };
        ~ClientImpl(){ };

        //ok: return 0
        //error: return -1
        int Set(std::string &key, std::string &value);
        int Get(std::string &key, std::string *value);
        int Del(std::string &key);

    public:
        Socket socket_;
        std::string buffer_;

};


#endif
