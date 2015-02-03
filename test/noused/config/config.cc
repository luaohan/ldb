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
#if 0
#define LDB_CONFIG_BUF_SIZE 4096

char *str_trim( char *istr, const char *str)
{
    assert(istr != NULL && str != NULL);

    int start_p = 0;
    int end_p = strlen(istr) - 1;

    while( strchr(str, istr[start_p]) != 0){
        start_p++;
    }

    while( strchr(str, istr[end_p]) != 0) {
        end_p--;
    }

    int len = end_p - start_p + 1;
    if( len <= 0 ) { 
        return NULL;
    }   

    memmove(istr, istr + start_p, len);
    istr[len] = '\0';

    return istr;
}

int strs2tokens(char *strs, const char *delim, char *tokens[], int *num)
{
    assert(strs != NULL && delim != NULL && tokens != NULL);

    char *token = strtok(strs, delim);

    int i = 0;
    while (token){
        if (token[0] == '\n') {
            break;
        }

        tokens[i++] = token;
        token = strtok(NULL, delim);
    }   
    tokens[i] = NULL;     

    if (num != NULL) *num = i;

    return 0;
}


Config::Config():
    daemon_(false), master_server_(false), server_port_(8899), level_(4),
    slave1_port_(8881), slave2_port_(8882)
{
    db_directory_ = "./db_directory";
    log_file_ = "./log/loginfo";
    //slave1_ip_ = "192.168.220.1";
    //slave2_ip_ = "192.168.220.2";
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
        tokens[i] = str_trim(tokens[i], " ");
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
        
        else if (strcmp(real_tokens[0], "master_server") == 0) {

            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }


            if (strcmp(real_tokens[1], "yes") == 0) {
           
                master_server_ = true;
            } else if (strcmp(real_tokens[1], "no") == 0) {
               
                master_server_ = false;
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
        
        else if (strcmp(real_tokens[0], "slave_port") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
       
            int port = atoi( real_tokens[1] );
           
            slave_port_ = port;
        } 
        
        else if (strcmp(real_tokens[0], "slave1_port") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
       
            int port = atoi( real_tokens[1] );
           
            slave1_port_ = port;
        } 
        
        else if (strcmp(real_tokens[0], "slave2_port") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
       
            int port = atoi( real_tokens[1] );
           
            slave2_port_ = port;
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
        
        else if (strcmp(real_tokens[0], "slave_ip") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
        
            slave_ip_ = real_tokens[1];
        } 
        
        else if (strcmp(real_tokens[0], "slave1_ip") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
        
            slave1_ip_ = real_tokens[1];
        } 
        
        else if (strcmp(real_tokens[0], "slave2_ip") == 0) {
            if (real_line_num != 2) {
                fprintf(stderr, " config file para error\n");
                return -1;
            }
        
            slave2_ip_ = real_tokens[1];
        } 
        
        else {
            fprintf(stderr, " config read error: %s\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}
#endif

int Config::LoadConfig(const std::string &filename)
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

    int node_num = json_object["node_num"].asInt();
    if (node_num <= 0 || node_num > 1000) {
        free(buf);
        close(fd);
        return -1;
    }
    virtual_server_.resize(node_num);

    std::list<int> all_nums;
    Socket *socket = NULL;
    Json::Value array = json_object["node_maps"];
    for (int i = 0; i < array.size(); i++)
    {
        Json::Value obj = array[i];
        Json::Value::Members member = obj.getMemberNames(); 
        Json::Value::Members::iterator iter = member.begin();

        for(; iter != member.end(); ++iter) 
        {
            std::string ip = obj[(*iter)]["ip"].asString();
            int port = obj[(*iter)]["port"].asInt();

            socket = new Socket(ip.c_str(), port);
            real_server_.push_back(socket);

            Json::Value num_array = obj[(*iter)]["virtual_node"];
            for (int j = 0; j < num_array.size(); j++)
            {       
                int num = num_array[j].asInt();
                if (num >= node_num) {
                    free(buf);
                    close(fd);
                    return -2;
                }

                virtual_server_[num] = socket;

                all_nums.push_back(num);
            }
        }
    }   

    if (all_nums.size() != node_num) {
        free(buf);
        close(fd);
        return -2;
    }   

    all_nums.sort();

    std::list<int>::iterator i;
    i = all_nums.begin();
    if (*i != 0) { //是否从0 开始
        free(buf);
        close(fd);
        return -2;
    } 

    int before = 0;
    i++; 
    for (; i != all_nums.end(); i++) { //是否连续
        if (*i - before != 1) {
            free(buf);
            close(fd);
            return -2;
        }
        before = *i;
    }

    bool ret = Connect();
    if (ret == false) {
        free(buf);
        close(fd);
        return -1;
    }
    
    hash_ = true;
    
    close(fd);
    free(buf);

    return 0;       
}
