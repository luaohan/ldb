// client_impl.h (2015-01-14)
// Yan Gaofeng (yangaofeng@360.cn)

#include <assert.h>
#include <vector>
#include <ldbc.h>
#include <net/socket.h>

class Client::Impl {
public:
    Impl(bool hash);
    ~Impl();

    //return -1, 错误
    //return -2, json 文件配置错误
    //return  0, 成功
    int Init(const std::string &file_name);

    bool Connect(const std::string &ip, int port);
    void Close();

    Status Set(const std::string &key, const std::string &value);
    Status Del(const std::string &key);
    Status Get(const std::string &key, std::string *value); 

private:
    unsigned int DJBHash(const unsigned char *buf, int len);
    bool Connect();
    Socket *GetSocket(const std::string &key);

private:
    Socket *socket_;
    std::vector<Socket *> server_;
    bool hash_; //是否采用分布式

};


