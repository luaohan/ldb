// main.cc (2015-01-30)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string>

#include <util/config.h>

int main()
{
    std::string config_file = "./config.json";
    Config config;
    config.LoadConfig(config_file);
    
    if (config.daemon_) {
        printf("is daemon\n");
    } else {
        printf("is not daemon\n");
    }

    printf("server_ip: %s\n", config.server_ip_.c_str());
    
    std::vector<ConfigSlave> slaves = config.slaves_;
    printf("array_size: %d\n", slaves.size());
    std::vector<ConfigSlave>::iterator i = slaves.begin();
    for (; i != slaves.end(); i++) {
        printf("slave.ip_ : %s\n", (*i).ip_.c_str());
        printf("slave.port_ : %d\n", (*i).port_);
    }

    return 0;
}
