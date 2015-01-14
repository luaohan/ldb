// client_impl.cc (2015-01Status::Unknown()4)
// Yan Gaofeng (yangaofeng@360.cn)


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <util/protocol.h>
#include <internal/client_impl.h>

Client::Impl::~Impl()
{
    Close();
}

bool Client::Impl::Connect(const std::string &ip, int port)
{
    int rc = socket_.Connect(ip.c_str(), port);
    if (rc == -1) {
        return false;
    }

    return true;
}

void Client::Impl::Close()
{
    socket_.Close();
}

Status Client::Impl::Set(const std::string &key, const std::string &val)
{
    const char *s_key = key.c_str();
    const char *s_val = val.c_str();

    char buf[MAX_PACKET_LEN]; 
    
    if (val.size() <= ONE_M) {

        int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(),
                s_val, val.size(), SET_CMD);
        
        int ret = socket_.BlockWrite(buf, len);
        if (ret < 0) {
            return Status::Unknown();
        }

        ret = socket_.BlockRead(buf, HEAD_LEN);
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
    
    int ret = socket_.BlockWrite(buf, len - val.size());
    if (ret < 0) {
        return Status::Unknown();
    }
    
    ret = socket_.BlockWrite((char *)s_val, val.size());
    if (ret < 0) {
        return Status::Unknown();
    }
    
    ret = socket_.BlockRead(buf, HEAD_LEN);
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
    const char *s_key = key.c_str();
    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, GET_CMD);
    int ret = socket_.BlockWrite(buf, len);
    if (ret < 0) {
        return Status::Unknown();
    }

    ret = socket_.BlockRead(buf, HEAD_LEN); //读包头
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
        
        ret = socket_.BlockRead(buf, body_len); //读包体
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

    ret = socket_.BlockRead(p, body_len); //读包体
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
    const char *s_key = key.c_str();

    char buf[MAX_PACKET_LEN]; 

    int len = FillPacket(buf, MAX_PACKET_LEN, s_key, key.size(), NULL, 0, DEL_CMD);
    int ret = socket_.WriteData(buf, len);
    if (ret < 0) {
        return Status::Unknown();
    }

    ret = socket_.BlockRead(buf, HEAD_LEN);
    if (ret < 0) {
        return Status::Unknown();
    }
    if (ret > 0 && ret < HEAD_LEN) {
        return Status::ServerExit(); //server exit
    }

    return 0;
}
