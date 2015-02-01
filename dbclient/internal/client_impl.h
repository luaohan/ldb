// client_impl.h (2015-01-14)
// Yan Gaofeng (yangaofeng@360.cn)

#include <vector>
#include <dbclient/client.h>
#include <net/socket.h>

class Client::Impl {
public:
    Impl(bool hash, const std::string &slave_conf);
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

    void InitSlaveInfo(const std::string &slave_conf);
    bool ConnectSlave1();
    bool ConnectSlave2();
    Status Get(const std::string &key, std::string *value, Socket *socket); 

private:
    Socket *socket_;
    std::vector<Socket *> real_server_;
    std::vector<Socket *> virtual_server_;
    bool hash_; //是否采用分布式

    bool master_server_exit_;
    Socket *socket_slave_1_;
    Socket *socket_slave_2_;
    
    std::string slave_1_ip_;
    std::string slave_2_ip_;

    int slave_1_port_;
    int slave_2_port_;
};


