// client_impl.cc (2015-01Status::Unknown()4)
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

Client::Impl::Impl(bool hash):
    socket_(NULL), hash_(hash) 
{
    if (!hash_) {
        socket_ = new Socket();
        assert(socket_ != NULL);
    }
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
            return Status::Unknown();
        }

        ret = socket_->BlockRead(buf, HEAD_LEN);
        if (ret < 0) {
            return Status::Unknown();
        }
        if (ret > 0 && ret < HEAD_LEN) {
            return Status::ServerExit();
        }

        return Status::Ok();
    }

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), 
            NULL, val.size(), SET_CMD);

    int ret = socket_->BlockWrite(buf, len - val.size());
    if (ret < 0) {
        return Status::Unknown();
    }

    ret = socket_->BlockWrite((char *)s_val, val.size());
    if (ret < 0) {
        return Status::Unknown();
    }

    ret = socket_->BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        return Status::Unknown();
    }
    if (ret > 0 && ret < HEAD_LEN) {
        return Status::ServerExit();
    }

    return Status::Ok();
}

Status Client::Impl::Get(const std::string &key, std::string *val)
{
    if (hash_) {
        socket_ = GetSocket(key);
    }

    const char *s_key = key.c_str();
    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, GET_CMD);
    int ret = socket_->BlockWrite(buf, len);
    if (ret < 0) {
        return Status::Unknown();
    }

    ret = socket_->BlockRead(buf, HEAD_LEN); //读包头
    if (ret < 0) {
        return Status::Unknown();
    }
    if (ret > 0 && ret < HEAD_LEN) {
        return Status::ServerExit(); //server exit
    }

    int packet_len = ntohl(*((int *)&(buf[0])));
    short packet_type = ntohs(*((short *)&(buf[sizeof(int)])));
    if (packet_type == REPLAY_NO_THE_KEY) {
        return Status::KeyNotExist();
    }

    int body_len = packet_len - HEAD_LEN;

    if (body_len <= ONE_M) {
        
        ret = socket_->BlockRead(buf, body_len); //读包体
        if (ret < 0) {
            return Status::Unknown();
        }
        if (ret > 0 && ret < HEAD_LEN) {
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
    if (ret > 0 && ret < HEAD_LEN) {
        free(p);
        return Status::ServerExit(); //server exit
    }

    //short value_len = ntohs(*((short *)&(buf[0])));
    int value_len = body_len - sizeof(short);

    std::string value(&p[sizeof(short)], value_len);
    *val = value;

    free(p);

    return 0;
}

Status Client::Impl::Del(const std::string &key)
{
    if (hash_) {
        socket_ = GetSocket(key);
    }
    
    const char *s_key = key.c_str();

    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, DEL_CMD);
    int ret = socket_->WriteData(buf, len);
    if (ret < 0) {
        return Status::Unknown();
    }

    ret = socket_->BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        return Status::Unknown();
    }
    if (ret > 0 && ret < HEAD_LEN) {
        return Status::ServerExit(); //server exit
    }

    return 0;
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
