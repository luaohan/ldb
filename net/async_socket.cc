// async_socket.cc (2014-12-24)
// WangPeng (1245268612@qq.com)

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>

#include "async_socket.h"


void AsyncSocket::Notify(int fd, short event, void *data)
{
    assert(data != NULL);

    AsyncSocket *s = (AsyncSocket *)data;

    if (fd == -1) {
        ////timeout
        ////没有连接成功，加一个时间事件，1 S 发一次连接
        //event_ = 
        //    event_new(base_, -1, EV_PERSIST, Server::ConnectSlaveCB, slave);
        //struct timeval t = {3, 0};
        //event_add(e, &t);
        //slave->set_time_event(e);
    }

    if (event & EV_READ) {
        if (!s->connecting_) {
            s->OnRead();
        } else {
            s->OnConnect();
        }
    } else if (event & EV_WRITE) {
        s->OnWrite();
    } else {
        assert(0);
    }
}

bool AsyncSocket::SetNonBlock()
{
    int flags;

    if ((flags = fcntl(fd_, F_GETFL, NULL)) < 0) {
        return false;
    }

    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
        return false; 
    }   

    return true;
}

int AsyncSocket::Connect()
{
    assert(handler_ != NULL);
    assert(fd_ == -1);

    //should be nonblock connect

    struct sockaddr_in addr;
    int result;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_.c_str());
    addr.sin_port = htons(port_);
    result = connect(fd_, (struct sockaddr *) & addr, sizeof(struct sockaddr)); 
    if ( result != 0 ) {
        return -1;
    }

    SetNonBlock();
    SetReuseAddr();
    SetNoNagle();

    Start();
    
    return 0;
}

int AsyncSocket::Read(char *buffer, int len)
{
    assert(buffer != NULL);
    assert(len > 0);

read_again:
    int bytes = read(fd_, buffer, len);
    if (bytes < 0) {
        if (errno == EINTR) {
            goto read_again;
        }

        return -1;
    }

    return bytes;
}

int AsyncSocket::Write(char *buffer, int len)
{
    assert(buffer != NULL);
    assert(len > 0);

write_again:
    int bytes = write(fd_, buffer, len);
    if (bytes == -1) {
        if (errno == EINTR) {
            goto write_again;
        }

        return -1;
    }

    return bytes;
}

void AsyncSocket::Close() 
{
    if (fd_ > 0) {
        if (event_ != NULL) {
            event_free(event_);
            event_ = NULL;
        }

        close(fd_);
        fd_ = -1;
    }
}

void AsyncSocket::OnRead()
{
    assert(handler_ != NULL);

    handler_(kRead, data_);
}

void AsyncSocket::OnWrite()
{
    assert(handler_ != NULL);

    handler_(kWrite, data_);
}

void AsyncSocket::OnConnect()
{
    assert(handler_ != NULL);

    connecting_ = false;

    event_free(event_);
    event_ = event_new(base_, fd_, EV_READ | EV_PERSIST,
            Notify, this);
    event_add(event_, NULL);

    handler_(kConnected, data_);
}

void AsyncSocket::OnTimeout()
{
    assert(handler_ != NULL);
    handler_(kTimeout, data_);
}

void AsyncSocket::Start()
{
    assert(handler_ != NULL);
    event_ = event_new(base_, fd_, EV_READ | EV_PERSIST,
            Notify, this);
    assert(event_ != NULL);
    event_add(event_, NULL);
}

void AsyncSocket::SetHandler(Handler handler, void *data)
{
    assert(handler != NULL);
    assert(data != NULL);
    handler_ = handler;
    data_ = data;
}

