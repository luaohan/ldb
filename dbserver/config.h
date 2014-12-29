// config.h (2014-12-28)
// WangPeng (1245268612@qq.com)

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>

namespace ldb {
namespace dbserver {

struct Config {
    
    Config();
    ~Config();

    //文件配置项全都正确无误，返回0
    //否则，返回 -1
    int LoadConfig(const char *filename);
    
    
    bool daemon_;

    int server_port_;
    
    std::string info_log_file_;

    std::string error_log_file_;

    std::string db_directory_;
    

    private:
        int LoadConfigFromStr(char *str);

};

} /*namespace ldb*/
} /*namespace dbserver*/

#endif

