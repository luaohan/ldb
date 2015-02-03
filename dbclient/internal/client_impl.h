// client_impl.h (2015-01-14)
// Yan Gaofeng (yangaofeng@360.cn)

#include <vector>
#include <dbclient/client.h>
#include <net/socket.h>

struct Server {
    Socket *master_server_; //当master_server 挂掉后，其为 NULL
    std::vector<Socket *> slave_server_;
};

class Client::Impl {
public:
    Impl(const std::string &conf_file);
    ~Impl();

    //return -1, 错误
    //return -2, json 文件配置错误
    //return  0, 成功
    int Init();
    
    //void Close();

    Status Set(const std::string &key, const std::string &value);
    Status Del(const std::string &key);
    Status Get(const std::string &key, std::string *value); 

private:
    unsigned int DJBHash(const unsigned char *buf, int len);
    bool ConnectMaster();
    Server GetServer(const std::string &key);

    Status Get(const std::string &key, std::string *value, 
            Socket *socket, bool master_exit);

private:
    std::vector<Server> real_server_;
    std::vector<Server> virtual_server_;
    
    std::string config_file_;
};
