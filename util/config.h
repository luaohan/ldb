// config.h (2014-12-28)
// WangPeng (1245268612@qq.com)

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>

//对 istr 左右两端进行修剪，清除其中 str 指定的所有字符
//比如 str_trim(xxyyabcyyxy, "xy") 将返回 "abc"  
char *str_trim( char *istr, const char *str);

//把str, 用delim 分开，将放在 tokens 数组中
//并且把最后的空格给去掉, *num 是 token 数组的大小
int strs2tokens(char *strs, const char *delim, char *tokens[], int *num);

struct Config {

    Config();
    ~Config();

    //文件配置项全都正确无误，返回0
    //否则，返回 -1
    int LoadConfig(const char *filename);

    bool daemon_;
    bool master_server_;

    int server_port_;

    std::string log_file_;

    std::string db_directory_;

    int level_;

    std::string slave_ip_;
    std::string slave1_ip_;
    std::string slave2_ip_;

    int slave_port_;
    int slave1_port_;
    int slave2_port_;

    private:
        int LoadConfigFromStr(char *str);

};

#endif
