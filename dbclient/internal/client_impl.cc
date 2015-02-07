// client_impl.cc (2015-01-14)
// Yan Gaofeng (yangaofeng@360.cn)

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <json/json.h>
#include <util/protocol.h>
#include <internal/client_impl.h>

Client::Impl::Impl(const std::string &config_file):
    config_file_(config_file)
{
    
}

Client::Impl::~Impl()
{
    std::vector<Server *>::iterator i = real_server_.begin();
    std::vector<Socket *>::iterator j;
    for (; i != real_server_.end(); i++) {

        if ((*i)->master_server_ != NULL) {
            delete (*i)->master_server_;
        }

        if (!(*i)->slave_server_.empty()) 
        {
            j = (*i)->slave_server_.begin();
            for (; j != (*i)->slave_server_.end(); j++) {
                delete (*j);
            }
        }

        delete (*i);
    }
}

//void Client::Impl::Close(){}

Status Client::Impl::Set(const std::string &key, const std::string &val)
{
    Server *s = GetServer(key);
    Socket *socket = s->master_server_;

    const char *s_key = key.c_str();
    const char *s_val = val.c_str();

    char buf[MAX_PACKET_LEN]; 

    if (val.size() <= ONE_M) {

        int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(),
                s_val, val.size(), SET_CMD);

        int ret = socket->BlockWrite(buf, len);
        if (ret < 0) {
            if (errno == 104) {
                return Status::ServerExit();
            }
            
            return Status::Unknown();
        }

        ret = socket->BlockRead(buf, HEAD_LEN);
        if (ret < 0) {
            if (errno == 104) {
                return Status::ServerExit();
            }
            return Status::Unknown();
        } else if (ret == 0) {
            return Status::ServerExit();
        }

        short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
        if (packet_type == REPLAY_ERROR) {
            return Status::Unknown();
        }

        return Status::Ok();
    }

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), 
            NULL, val.size(), SET_CMD);

    int ret = socket->BlockWrite(buf, len - val.size());
    if (ret < 0) {
        if (errno == 104) {
            return Status::ServerExit();
        }
    
        return Status::Unknown();
    }

    ret = socket->BlockWrite((char *)s_val, val.size());
    if (ret < 0) {
        if (errno == 104) {
            return Status::ServerExit();
        }
    
        return Status::Unknown();
    }

    ret = socket->BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        if (errno == 104) {
            return Status::ServerExit();
        }
        return Status::Unknown();
    } else if (ret == 0) {
        return Status::ServerExit();
    }

    short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
    if (packet_type == REPLAY_ERROR) {
        return Status::Unknown();
    }

    return Status::Ok();
}

Status Client::Impl::Get(const std::string &key, std::string *val)
{
    Status status;
    bool master_exit;
    
    Server *server = GetServer(key);
    Socket *socket;
    
get_again:
    if (server->master_server_ == NULL) { 
        //表示 master_server 挂了
        //获取最后一个slave
        socket = server->slave_server_.back();
        master_exit = true;
    } else {

        socket = server->master_server_;
        master_exit = false;
    }

    status = Get(key, val, socket, master_exit);
    if (status.IsServerExit()) 
    {
        if (server->master_server_ == NULL) 
        {
            delete server->slave_server_.back();
            server->slave_server_.pop_back();
        } else {

            delete server->master_server_;
            server->master_server_ = NULL;
        }
again:
        if (server->slave_server_.empty()) {
            return Status::ServerExit();
        }
        
        //返回最后一个元素
        socket = server->slave_server_.back();
        int ret = socket->Connect();
        if (ret == -1) {
            //有可能连接slave 失败
            //有几个slave 就有几次Get 的机会
            delete server->slave_server_.back();
            server->slave_server_.pop_back();
            goto again;
        }
        
        goto get_again;
    } 
    else {
        return status;
    }
}

Status Client::Impl::Get(const std::string &key, std::string *val, 
        Socket *socket, bool master_exit)
{
    assert(socket != NULL);

    const char *s_key = key.c_str();
    char buf[MAX_PACKET_LEN]; 

    int len = 
        FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, GET_CMD);
    
    if (master_exit) {
        len += sizeof(short);
    }
    
    int ret = socket->BlockWrite(buf, len);
    if (ret < 0) {
        if (errno == 104) {
            return Status::ServerExit();
        }

        return Status::Unknown();
    }
    
    ret = socket->BlockRead(buf, HEAD_LEN); //读包头
    if (ret < 0) {
        if (errno == 104) {
            return Status::ServerExit();
        }
        return Status::Unknown();
    } else if (ret == 0) {
        return Status::ServerExit(); //server exit
    }

    short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
    if (packet_type == REPLAY_NO_THE_KEY) {
        return Status::KeyNotExist();
    }

    int packet_len = ntohl(*((int *)&(buf[0])));
    int body_len = packet_len - HEAD_LEN;

    if (body_len <= ONE_M) {

        ret = socket->BlockRead(buf, body_len); //读包体
        if (ret < 0) {
            if (errno == 104) {
                return Status::ServerExit();
            }
            return Status::Unknown();
        } else if (ret == 0) {
            return Status::ServerExit();
        }

        //short value_len = ntohs(*((short *)&(buf[0])));
        int value_len = body_len - sizeof(short);

        std::string value(&buf[sizeof(short)], value_len);
        *val = value;
       
        return Status::Ok();
    }

    char *p = (char *)malloc(body_len);
    if (p == NULL) {
        return Status::Unknown();
    }

    ret = socket->BlockRead(p, body_len); //读包体
    if (ret < 0) {
        free(p);
        if (errno == 104) {
            return Status::ServerExit();
        }
        return Status::Unknown();
    } else if (ret == 0) {
        free(p);
        return Status::ServerExit(); //server exit
    }

    //short value_len = ntohs(*((short *)&(buf[0])));
    int value_len = body_len - sizeof(short);

    std::string value(&p[sizeof(short)], value_len);
    *val = value;

    free(p);

    return Status::Ok();
}

Status Client::Impl::Del(const std::string &key)
{
    Server *s = GetServer(key);
    Socket *socket = s->master_server_;
    
    const char *s_key = key.c_str();

    char buf[MAX_PACKET_LEN]; 
    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, DEL_CMD);
    int ret = socket->BlockWrite(buf, len);
    if (ret < 0) {
        if (errno == 104) {
            return Status::ServerExit();
        }
    
        return Status::Unknown();
    }

    ret = socket->BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        if (errno == 104) {
            return Status::ServerExit();
        }
        return Status::Unknown();
    } else if (ret == 0) {
        return Status::ServerExit(); //server exit
    }

    short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
    if (packet_type == REPLAY_ERROR) {
        return Status::Unknown();
    }

    return Status::Ok();
}

int Client::Impl::Init()
{
    int fd = open(config_file_.c_str(), O_RDONLY);
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
        return -2;
    }
    
    virtual_server_.resize(node_num);

    std::list<int> all_nums;
    
    Server *server = NULL;

    Json::Value array = json_object["node_maps"];
    for (int i = 0; i < array.size(); i++)
    {
        server = new Server;
       
        Json::Value obj = array[i];
        Json::Value::Members member = obj.getMemberNames(); 
        Json::Value::Members::iterator iter = member.begin();

        for(; iter != member.end(); ++iter) 
        {
            std::string ip = obj[(*iter)]["ip"].asString();
            int port = obj[(*iter)]["port"].asInt();

            server->master_server_ = new Socket(ip.c_str(), port);

            Json::Value slave_array = obj[(*iter)]["slave"];
            for (int j = 0; j < slave_array.size(); j++) 
            {
                ip = slave_array[j]["ip"].asString();
                port = slave_array[j]["port"].asInt(); 
                server->slave_server_.push_back(new Socket(ip.c_str(), port));
            }
            
            real_server_.push_back(server);

            Json::Value num_array = obj[(*iter)]["virtual_node"];
            for (int j = 0; j < num_array.size(); j++)
            {       
                int num = num_array[j].asInt();
                if (num >= node_num) {
                    free(buf);
                    close(fd);
                    return -2;
                }

                virtual_server_[num] = server;

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

    //去连接所有的 master_server
    bool ret = ConnectMaster();
    if (ret == false) {
        free(buf);
        close(fd);
        return -1;
    }
    
    close(fd);
    free(buf);

    return 0;       
}

bool Client::Impl::ConnectMaster()
{
    std::vector<Server *>::iterator i = real_server_.begin();
    int ret;
    Socket *s;
    for (; i != real_server_.end(); i++) {
        s = (*i)->master_server_;
        ret = s->Connect();
        if (ret == -1) {
            return false;
        }
    }

    return true;
}


Server *Client::Impl::GetServer(const std::string &key)
{
    int virtual_node =
        DJBHash((const unsigned char *)key.c_str(), key.size()) % virtual_server_.size();

    return virtual_server_[virtual_node];
}


unsigned int Client::Impl::DJBHash(const unsigned char *buf, int len)
{
    static const int hash_function_seed = 5381;
    unsigned int hash = (unsigned int)hash_function_seed;
    while (len--) {
        hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
    }

    return hash;
}                

