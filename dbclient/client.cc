// ldbc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <internal/client_impl.h>
#include <assert.h>

Client::Client(bool hash, const std::string &slave_conf) : 
    impl_(new Impl(hash, slave_conf))
{
    assert(impl_ != NULL);
}

Client::~Client()
{
    if (impl_ != NULL) {
        delete impl_;
        impl_ = NULL;
    }
}

int Client::Init(const std::string &file_name)
{
    return impl_->Init(file_name);
}

bool Client::Connect(const std::string &ip, int port)
{
    return impl_->Connect(ip, port);
}

void Client::Close()
{
    impl_->Close();
}

Status Client::Set(const std::string &key, const std::string &value)
{
    return impl_->Set(key, value);
}

Status Client::Del(const std::string &key)
{
    return impl_->Del(key);
}

Status Client::Get(const std::string &key, std::string *value)
{
    return impl_->Get(key, value);
}

