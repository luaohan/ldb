// config.h (2014-12-28)
// WangPeng (1245268612@qq.com)

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <vector>

struct ConfigSlave {
    std::string ip_;
    int port_;
};

struct Config {

    Config(){};
    ~Config(){};

    //文件配置项全都正确无误，返回0
    //如果json 文件错误，返回 -2
    //其它错误，返回 -1
    int LoadConfig(const std::string &filename);

    bool daemon_;
    bool master_server_;

    std::string server_ip_;
    int server_port_;

    std::vector<ConfigSlave> slaves_;
    
    std::string log_file_;
    std::string db_directory_;
    int level_;

};

#endif
