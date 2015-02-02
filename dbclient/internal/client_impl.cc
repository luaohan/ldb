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

Client::Impl::Impl(bool hash, const std::string &slave_conf):
    socket_(NULL), hash_(hash), master_server_exit_(false),
    socket_slave_1_(NULL), socket_slave_2_(NULL)
{
    if (!hash_) {
        socket_ = new Socket();
        assert(socket_ != NULL);
    }
    
    InitSlaveInfo(slave_conf);
}

Client::Impl::~Impl()
{
    if (!hash_) {
        delete socket_;
    }

    if (hash_) {
        std::vector<Socket *>::iterator i;
        for (i = real_server_.begin(); i != real_server_.end(); i++) {
            delete (*i);
        }
    }

    if (socket_slave_1_ != NULL) {
        delete socket_slave_1_;
    }

    if (socket_slave_2_ != NULL) {
        delete socket_slave_2_;
    }
}

bool Client::Impl::Connect(const std::string &ip, int port)
{
    int rc = socket_->Connect(ip.c_str(), port);
    if (rc == -1) {
        return false;
    }

    return true;
}

void Client::Impl::Close()
{
    if (!hash_) {
        socket_->Close();
    }
}

Status Client::Impl::Set(const std::string &key, const std::string &val)
{
    if (master_server_exit_) {
        return Status::ServerExit();
    }
    
    if (hash_) {
        socket_ = GetSocket(key);
    }

    const char *s_key = key.c_str();
    const char *s_val = val.c_str();

    char buf[MAX_PACKET_LEN]; 

    if (val.size() <= ONE_M) {

        int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(),
                s_val, val.size(), SET_CMD);

        int ret = socket_->BlockWrite(buf, len);
        if (ret < 0) {
            if (errno == 104) {
                master_server_exit_ = true;
                return Status::ServerExit();
            }
            
            return Status::Unknown();
        }

        ret = socket_->BlockRead(buf, HEAD_LEN);
        if (ret < 0) {
            return Status::Unknown();
        }
        if ((ret > 0 && ret < HEAD_LEN) || ret == 0) {
            master_server_exit_ = true;
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

    int ret = socket_->BlockWrite(buf, len - val.size());
    if (ret < 0) {
        if (errno == 104) {
            master_server_exit_ = true;
            return Status::ServerExit();
        }
    
        return Status::Unknown();
    }

    ret = socket_->BlockWrite((char *)s_val, val.size());
    if (ret < 0) {
        if (errno == 104) {
            master_server_exit_ = true;
            return Status::ServerExit();
        }
    
        return Status::Unknown();
    }

    ret = socket_->BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        return Status::Unknown();
    }
    if ((ret > 0 && ret < HEAD_LEN) || ret == 0) {
        master_server_exit_ = true;
        return Status::ServerExit();
    }
    
    short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
    if (packet_type == REPLAY_ERROR) {
        return Status::Unknown();
    }

    return Status::Ok();
}

Status Client::Impl::Get(const std::string &key, std::string *val, Socket *socket)
{
    assert(socket != NULL);

    const char *s_key = key.c_str();
    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, GET_CMD);
    if (master_server_exit_) {
        len += sizeof(short);
    }

    int ret = socket->BlockWrite(buf, len);
    if (ret < 0) {
        if (errno == 104) {
            master_server_exit_ = true;
            return Status::ServerExit();
        }
    
        return Status::Unknown();
    }
    ret = socket->BlockRead(buf, HEAD_LEN); //读包头
    if (ret < 0) {
        return Status::Unknown();
    }
    if ((ret > 0 && ret < HEAD_LEN) || ret == 0) {
        master_server_exit_ = true;
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
            return Status::Unknown();
        }
        if ((ret > 0 && ret < body_len)|| ret == 0) {
            master_server_exit_ = true;
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
        return Status::Unknown();
    }
    if ((ret > 0 && ret < HEAD_LEN) || ret == 0) {
        master_server_exit_ = true;
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

Status Client::Impl::Get(const std::string &key, std::string *val)
{
    if (master_server_exit_) {
        if (socket_slave_1_ == NULL) {
            ConnectSlave1(); //这里先保证slave1 一定存在
        }

        return Get(key, val, socket_slave_1_);
    }
   
    if (hash_) {
        socket_ = GetSocket(key);
    }

    return Get(key, val, socket_);

#if 0
    const char *s_key = key.c_str();
    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, GET_CMD);
    int ret = socket_->BlockWrite(buf, len);
    if (ret < 0) {
        if (errno == 104) {
            master_server_exit_ = true;
            return Status::ServerExit();
        }
    
        return Status::Unknown();
    }
    ret = socket_->BlockRead(buf, HEAD_LEN); //读包头
    if (ret < 0) {
        return Status::Unknown();
    }
    if ((ret > 0 && ret < HEAD_LEN) || ret == 0) {
        master_server_exit_ = true;
        return Status::ServerExit(); //server exit
    }

    short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
    if (packet_type == REPLAY_NO_THE_KEY) {
        return Status::KeyNotExist();
    }

    int packet_len = ntohl(*((int *)&(buf[0])));
    int body_len = packet_len - HEAD_LEN;

    if (body_len <= ONE_M) {
        
        ret = socket_->BlockRead(buf, body_len); //读包体
        if (ret < 0) {
            return Status::Unknown();
        }
        if ((ret > 0 && ret < HEAD_LEN )|| ret == 0) {
            master_server_exit_ = true;
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

    ret = socket_->BlockRead(p, body_len); //读包体
    if (ret < 0) {
        free(p);
        return Status::Unknown();
    }
    if ((ret > 0 && ret < HEAD_LEN) || ret == 0) {
        master_server_exit_ = true;
        free(p);
        return Status::ServerExit(); //server exit
    }

    //short value_len = ntohs(*((short *)&(buf[0])));
    int value_len = body_len - sizeof(short);

    std::string value(&p[sizeof(short)], value_len);
    *val = value;

    free(p);

    return Status::Ok();
#endif

}

Status Client::Impl::Del(const std::string &key)
{
    if (master_server_exit_) {
        return Status::ServerExit();
    }
    
    if (hash_) {
        socket_ = GetSocket(key);
    }
    
    const char *s_key = key.c_str();

    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, DEL_CMD);
    int ret = socket_->BlockWrite(buf, len);
    if (ret < 0) {
        if (errno == 104) {
            master_server_exit_ = true;
            return Status::ServerExit();
        }
    
        return Status::Unknown();
    }

    ret = socket_->BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        return Status::Unknown();
    }
    if ((ret > 0 && ret < HEAD_LEN) || ret == 0) {
        master_server_exit_ = true;
        return Status::ServerExit(); //server exit
    }
    
    short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
    if (packet_type == REPLAY_ERROR) {
        return Status::Unknown();
    }

    return Status::Ok();
}

int Client::Impl::Init(const std::string &file_name)
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

bool Client::Impl::Connect()
{
    std::vector<Socket *>::iterator i;
    int ret;
    for (i = real_server_.begin(); i != real_server_.end(); i++) {
        ret = (*i)->Connect();
        if (ret == -1) {
            return false;
        }
    }

    return true;
}


Socket *Client::Impl::GetSocket(const std::string &key)
{
    int virtual_node =
        DJBHash((const unsigned char *)key.c_str(), key.size()) % virtual_server_.size();

    Socket *s = virtual_server_[virtual_node];

    return s;
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

void Client::Impl::InitSlaveInfo(const std::string &slave_conf)
{
    int fd = open(slave_conf.c_str(), O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "open slave conf error: %s\n", strerror(errno));
        exit(0);
    }
    
    int BUFSIZE = 1024 * 1024 * 1; //1M
    char *buf = (char *)malloc(BUFSIZE);
    if (buf == NULL) {
        close(fd);
        fprintf(stderr, "malloc error: %s\n", strerror(errno));
        exit(0);
    }
    
    int read_size = read(fd, buf, BUFSIZE);
    if (read_size < 0) {
        free(buf);
        close(fd);
        fprintf(stderr, "read error: %s\n", strerror(errno));
        exit(0);
    }

    buf[read_size] = '\0';

    Json::Reader reader;
    Json::Value json_object;
    if (!reader.parse(buf, json_object)) {
        free(buf);
        close(fd);
        fprintf(stderr, "json file error\n");
        exit(0);
    }

    slave_1_ip_ = json_object["slave_1_ip"].asString();
    slave_2_ip_ = json_object["slave_2_ip"].asString();
    
    slave_1_port_ = json_object["slave_1_port"].asInt();
    slave_2_port_ = json_object["slave_2_port"].asInt();
    
    close(fd);
    free(buf);

    return ;
}

bool Client::Impl::ConnectSlave1()
{
    socket_slave_1_ = new Socket();
    assert(socket_slave_1_ != NULL);

    if (socket_slave_1_->Connect(slave_1_ip_.c_str(), slave_1_port_) == -1) {
        socket_slave_1_->Close();
        delete socket_slave_1_;
        socket_slave_1_ = NULL;
        return false;
    }

    return true;
}

bool Client::Impl::ConnectSlave2()
{
    socket_slave_2_ = new Socket();
    assert(socket_slave_2_ != NULL);

    if (socket_slave_2_->Connect(slave_1_ip_.c_str(), slave_1_port_) == -1) {
        socket_slave_2_->Close();
        delete socket_slave_2_;
        socket_slave_2_ = NULL;
        return false;
    }
    
    return true;
}
