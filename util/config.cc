// config.cc (2014-12-28)
// WangPeng (1245268612@qq.com)

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#include <json/json.h>
#include "config.h"

int Config::LoadConfig(const std::string &file_name)
{
    int fd = open(file_name.c_str(), O_RDONLY);
    if (fd < 0) {
        return -1;                         
    }
    
    int BUFSIZE = 1024 * 1024 * 10; //10M
    char *buf = (char *)malloc(BUFSIZE);
    if (buf == NULL) {
        close(fd);
        return -1;
    }
    
    int read_size = read(fd, buf, BUFSIZE);
    if (read_size < 0) {
        free(buf);
        close(fd);
        return -1;
    }

    buf[read_size] = '\0';

    Json::Reader reader;
    Json::Value json_object;
    if (!reader.parse(buf, json_object)) {
        free(buf);
        close(fd);
        return -2;
    }
    
    master_server_ = json_object["is_master"].asBool();

    if (master_server_) {
        Json::Value master_server = json_object["master_server"];
        server_ip_ = master_server["ip"].asString();
        server_port_ = master_server["port"].asInt();
        daemon_ = master_server["deamonize"].asBool();
        log_file_ = master_server["logfile"].asString();
        db_directory_ = master_server["db_directory"].asString();
        level_ = master_server["log_level"].asInt();

        Json::Value slave_array = master_server["slave"];
        for (int i = 0; i < slave_array.size(); i++)
        {

            Json::Value obj = slave_array[i];
            
            ConfigSlave slave;
            slave.ip_ = obj["ip"].asString();
            slave.port_ = obj["port"].asInt();
            slaves_.push_back(slave);
        }
    } else {
        Json::Value slave_server = json_object["slave_server"];
        server_ip_ = slave_server["ip"].asString();
        server_port_ = slave_server["port"].asInt();
        daemon_ = slave_server["deamonize"].asBool();
        log_file_ = slave_server["logfile"].asString();
        db_directory_ = slave_server["db_directory"].asString();
        level_ = slave_server["log_level"].asInt();
    }

    free(buf);
    close(fd);
    
    return 0;       
}
