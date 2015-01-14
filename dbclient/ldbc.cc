// ldbc.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <internal/client_impl.h>

Client::Client() : impl_(new Impl)
{
}

Client::~Client()
{
    if (impl_ != NULL) {
        delete impl_;
        impl_ = NULL;
    }
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
    return Get(key, value);
}

