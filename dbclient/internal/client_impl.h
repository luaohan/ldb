// client_impl.h (2015-01-14)
// Yan Gaofeng (yangaofeng@360.cn)

#include <ldbc.h>
#include <net/socket.h>

class Client::Impl {
public:
    Impl() {}
    ~Impl();

    bool Connect(const std::string &ip, int port);
    void Close();

    Status Set(const std::string &key, const std::string &value);
    Status Del(const std::string &key);
    Status Get(const std::string &key, std::string *value); 

private:
    Socket socket_;
};


