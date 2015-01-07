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

#include "config.h"
#include "str.h"

#define LDB_CONFIG_BUF_SIZE 4096

Config::Config():
    daemon_(false), server_port_(8899), level_(4)
{
    db_directory_ = "./db_directory";
    log_file_ = "./log/loginfo";
}

Config::~Config()
{
}

int Config::LoadConfig(const char *filename)
{
    assert(filename != NULL);

    int ret;
    int file_fd;
    int read_len;
    char str[LDB_CONFIG_BUF_SIZE * 10]; //将配置文件中的所有的内容都存放在 str 中
    char buf[LDB_CONFIG_BUF_SIZE];
    
    file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        fprintf(stderr, "config open error: %s\n", strerror(errno));
        return -1;
    }
    
    while ( 1 ) {
read_again:
        read_len = read(file_fd, buf, LDB_CONFIG_BUF_SIZE);
        if (read_len < 0) {
            if (errno == EINTR) goto read_again;
            fprintf(stderr, "config read error: %s\n", strerror(errno));
            return -1;
        }
        
        buf[read_len] = '\0';

        strcpy(str, buf);
       
        if (read_len < LDB_CONFIG_BUF_SIZE)
            break;
    }
    
    ret = LoadConfigFromStr(str);
   
    return ret;
}

int Config::LoadConfigFromStr(char *str)
{
    int line_num;
    int real_line_num;
    char *tokens[100];
    char *real_tokens[100];
    int i;
    int j = 0;

    strs2tokens(str, "\n", tokens, &line_num);
    for (i = 0; i < line_num; i++)
    {
        tokens[i] = ldb_str_trim(tokens[i], " ");
        if (tokens[i][0] == '#' || tokens[i] == NULL) {
            continue;
        }
   
        strs2tokens(tokens[i], " ", real_tokens, &real_line_num);
        if (strcmp(real_tokens[0], "daemonize") == 0) {
           
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
            

            if (strcmp(real_tokens[1], "yes") == 0) {
           
                daemon_ = true;
            } else if (strcmp(real_tokens[1], "no") == 0) {
               
                daemon_ = false;
            } else {
               
                fprintf(stderr, " config file para error \n");
                return -1;
            }
        } 
        
        else if (strcmp(real_tokens[0], "server_port") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
       
            int port = atoi( real_tokens[1] );
           
            server_port_ = port;
        } 
        
        else if (strcmp(real_tokens[0], "level") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
       
            int level = atoi( real_tokens[1] );
           
            level_ = level;
        } 
        
        else if (strcmp(real_tokens[0], "logfile") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
            
            log_file_ = real_tokens[1];
        }
        
        else if (strcmp(real_tokens[0], "db_directory") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
        
            db_directory_ = real_tokens[1];
        } 
        
        else {
            fprintf(stderr, " config read error: %s\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}
