// ldb_server.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <time.h>

#include <net/acceptor.h>
#include <util/daemon.h>
#include <util/log.h>

#include <dbserver/server.h>
#include <dbserver/client.h>
#include <dbserver/slave.h>
#include <dbserver/signal.h>

bool quit = false;

Server::Server():
    socket_(NULL), slave1_(NULL), slave2_(NULL), event_(NULL), base_(NULL),
    slave_1_(NULL), server_can_write_(false)//, time_out_(2000) //2 S
{

}

Server::~Server()
{
    if (socket_ != NULL) {
        delete socket_;
    }
    
    if (slave1_ != NULL) {
        delete slave1_;
    }
    
    if (slave2_ != NULL) {
        delete slave2_;
    }

    if (event_ != NULL) {
        event_free(event_);
    }

    if (base_ != NULL) {
        event_base_free(base_);
    }
    
    delete db_;
}

int Server::Insert(const leveldb::Slice& key, const leveldb::Slice& value)
{
    leveldb::Status status;
    status = db_->Put(write_options_, key, value);
    if (status.ok())
        return 0;
    else 
        return -1; //一般不会发生
}

int Server::Get(const leveldb::Slice& key, std::string* value)
{
    leveldb::Status status;
    status = db_->Get(read_options_, key, value);
    if (status.ok())
        return 0;
    else 
        return -1;  //没有这个 key
}

int Server::Delete(const leveldb::Slice& key)
{
    leveldb::Status status;
    status = db_->Delete(write_options_, key);
    if (status.ok())
        return 0;
    else 
        return -1; //一般不会发生
}

void Server::AddClient(Client *cli)
{
    clients_.push_back(cli);
}

void Server::DeleteClient(int fd)
{
    std::vector<Client *>::iterator i;
    for (i = clients_.begin(); i != clients_.end(); i++)
    {
        if ((*i)->fd() == fd) {
            clients_.erase(i);
            break;
        }
    }
}

Client *Server::FindClient(int fd)
{
    std::vector<Client *>::iterator i;
    for (i = clients_.begin(); i != clients_.end(); i++)
    {
        if ((*i)->fd() == fd) {
            return (*i);
        }
    }
}

int Server::Run(const char *config_file)
{
    int ret = config_.LoadConfig(config_file);
    if (ret != 0) {
        fprintf(stderr, "configfile is wrong.\n");
        return -1;
    }

    log = new Log(config_.log_file_, config_.level_, 0);
    if (log->fd() == -1 ) {
        fprintf(stderr, "open logfile error: %s\n", strerror(errno));
        return -1; 
    }

    options_.create_if_missing = true;
    leveldb::Status status 
        = leveldb::DB::Open(options_, config_.db_directory_.c_str(), &db_);
    assert(status.ok());

    ret = CreateServer();
    if (ret < 0) {
        return -1;
    }

    if (log != NULL) {
        delete log;
    }

    return 0;
}

int Server::CreateServer()
{
    socket_ = new Acceptor;
    if (socket_ == NULL) {
        return -1;
    }
    
    int backlog = 512;
    if (config_.master_server_) {
        socket_->SetReuseAddr();
        if (socket_->Listen("0.0.0.0", config_.server_port_, backlog) < 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }
        
        slave1_ = new Socket;
        if (slave1_ == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }

        //slave1_->SetNonBlock();
        slave_1_ = new Slave(slave1_, this, NULL, NULL);  //这里只先弄一个 slave
        if (slave_1_ == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }

        slave2_ = new Socket;
        if (slave2_ == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }

        //slave2_->SetNonBlock();

    } else {
        socket_->SetReuseAddr();
        if (socket_->Listen(config_.slave_ip_.c_str(), config_.slave_port_, backlog) < 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }
    }

    socket_->SetNonBlock();

#if 0
    Event e;
    e.fd_ = socket_->fd();
    e.ptr_ = socket_;
    event_.AddReadEvent(e);
#endif
    
    base_ = event_base_new();
    assert(base_ != NULL);
    event_ = event_new(base_, socket_->fd(),
                EV_READ | EV_PERSIST, Server::ListenCB, this);
    assert(event_ != NULL);
    int ret = event_add(event_, NULL);
    assert(ret != -1);

    if (config_.master_server_) {
        ConnectSlave();
    } else {
        server_can_write_ = true;
        //time_out_ = -1;
    }

    if ( config_.daemon_ ) {
        Daemon();
    }

    //fprintf(stderr, "init server success\n");

    event_base_dispatch(base_);

#if 0
    while (!quit) {
        ProcessEvent();
    }
#endif

    return 0;
}

#if 0
int Server::ProcessEvent()
{
    int n = event_.WaitReadEvent(fired_read_, fired_write_, time_out_);
    if ( n < 0) {
        log_error("WaitReadEvent error:[%s]", strerror(errno));
        return -1;
    }
    
    //读事件
    ProcessReadEvent();
    
    //写事件
    ProcessWriteEvent();

    //时间事件
    if (server_can_write_ == false) {
        ProcessTimeEvent();
    }

    return 0;
}                                        

void Server::ProcessReadEvent()
{
    for (int i = 0; fired_read_[i].ptr_ != NULL; i++) 
    {
        if ( (Acceptor *)fired_read_[i].ptr_ == socket_) {
            Socket *link = socket_->Accept();
            if (link == NULL) {
                log_error("Accept error:[%s]", strerror(errno));
                continue;
            }

            link->SetNonBlock();

            Client *cli = new Client(this, link);
            AddClient(cli);
#if 0 
            Event e;
            e.fd_ = link->fd();
            e.ptr_ = cli;
            event_.AddReadEvent(e);
#endif    
            log_info("---<create a client:[ip:%s],[port:%d],[fd:%d]>---", 
                    link->ip(), link->port(), link->fd());

            continue;     
        }

        if ((Slave *)fired_read_[i].ptr_ == slave_1_) {
            int rc = slave_1_->Read();
            
            if (rc == 2) {
                continue;
            }

            //到这里表示读完了slave 发来的信息
            continue;
        }

        Client *cli = (Client *)fired_read_[i].ptr_;
        int rc = cli->Read(slave_1_);
        if (rc == -1) {
            DeleteClient(cli);//close the client
        }
    }
}

void Server::ProcessWriteEvent()
{
    for (int i = 0; fired_write_[i].ptr_ != NULL; i++) 
    {
        if ((Slave *)fired_write_[i].ptr_ == slave_1_) {
            int rc = slave_1_->Write();
            if (rc == 2) {
                continue;
            }
#if 0
            //到这里表示写完了要发给slave 的信息
            Event e;
            e.fd_ = slave_1_->link_->fd();
            e.ptr_ = slave_1_;
            event_.DelWriteEvent(e);
#endif
            continue;
        }

        Client *cli = (Client *)fired_write_[i].ptr_;
        int rc = cli->Write();
        if (rc == -1) {
            DeleteClient(cli);//close the client
        } else if (rc == 2) {
            continue;
        }

        //到这里说明已经写完了
        //移除可写事件
#if 0
        Event e;
        e.fd_ = cli->fd();
        e.ptr_ = cli;
        event_.DelWriteEvent(e);
#endif
    }
}

void Server::ProcessTimeEvent()
{
    std::vector<TimeEvent>::iterator i = time_event_.begin();

    struct timeval tv;
    unsigned long long now_time;        
    int ret;

    for (; i != time_event_.end(); i++) {
        if (time_event_.size() == 0) {
            break;
        }
        gettimeofday(&tv, NULL);
        now_time = tv.tv_sec * 1000000 + tv.tv_usec;
        if (now_time >= (*i).time_) {
            ret = (*i).s_->Connect((*i).ip_.c_str(), (*i).port_);
            if (config_.daemon_ == false) {
                fprintf(stderr, "connect %s, %d\n", (*i).ip_.c_str(), (*i).port_);
            }
            
            if (ret == -1) {
                gettimeofday(&tv, NULL);
                (*i).time_ = (tv.tv_sec + 2) * 1000000 + tv.tv_usec;
                continue;
            }
            
            time_event_.erase(i);   //移除
        }
    }

    if(time_event_.empty()) {
        server_can_write_ = true;
        //time_out_ = -1;
#if 0
        Event e;
        e.fd_ = slave1_->fd();
        e.ptr_ = slave_1_;
        event_.AddReadEvent(e);
#endif
    }
}
#endif

void Server::DeleteClient(Client *c)
{
#if 0
    Event e;
    e.fd_ = c->fd();
    event_.DelReadEvent(e);
#endif
  
    DeleteClient(c->fd());
    
    delete c;
}

void Server::ConnectSlave()
{
    int ret = -1;
    ret = slave1_->Connect(config_.slave1_ip_.c_str(), config_.slave1_port_);
    fprintf(stderr, "ip: %s, port: %d\n", config_.slave1_ip_.c_str(), config_.slave1_port_);
    fprintf(stderr, "connect error: %s, %d\n", strerror(errno), errno);
    assert(ret != -1);
#if 0
    if (ret == -1) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        //process after 2 second
        TimeEvent te(slave1_, (tv.tv_sec + 2) * 1000000 + tv.tv_usec, config_.slave1_ip_, config_.slave1_port_);
        time_event_.push_back(te);
    }
#endif
    
    ret = slave2_->Connect(config_.slave2_ip_.c_str(), config_.slave2_port_);
    assert(ret != -1);
#if 0
    if (ret == -1) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        //process after 2 second
        TimeEvent te(slave2_, (tv.tv_sec + 2) * 1000000 + tv.tv_usec, config_.slave2_ip_, config_.slave2_port_);
        time_event_.push_back(te);
    }
#endif

    //if (time_event_.empty()) {
        server_can_write_ = true;
        //time_out_ = -1;
        struct event *read_event = 
            event_new(base_, slave1_->fd(), 
                    EV_READ | EV_PERSIST, Server::SlaveReadCB, slave_1_);
        assert(read_event != NULL);
        ret = event_add(read_event, NULL);
        assert(ret != -1);
        slave_1_->set_read_event(read_event);
#if 0
        Event e;
        e.fd_ = slave1_->fd();
        e.ptr_ = slave_1_;
        event_.AddReadEvent(e);
#endif

    //}
}

void Server::ListenCB(int fd, short what, void *arg)
{
    Server *server = (Server *)arg;
    Socket *link = server->socket_->Accept();
    if (link == NULL) {
        log_error("Accept error:[%s]", strerror(errno));
        return ;
    }

    link->SetNonBlock();
    
    Client *cli = new Client(server, link, server->slave_1_, NULL, NULL);
    server->AddClient(cli);
    
    struct event *read_event = 
        event_new(server->base_, link->fd(), EV_READ | EV_PERSIST, Server::ClientReadCB, cli);
    assert(read_event != NULL);
    int ret = event_add(read_event, NULL);
    assert(ret != -1);

    cli->set_read_event(read_event);

#if 0
    Event e;
    e.fd_ = link->fd();
    e.ptr_ = cli;
    event_.AddReadEvent(e);
#endif

    log_info("---<create a client:[ip:%s],[port:%d],[fd:%d]>---", 
            link->ip(), link->port(), link->fd());

    return ;
}

void Server::ClientReadCB(int fd, short what, void *arg)
{
    Client *cli = (Client *)arg;
    int rc = cli->Read(/*slave_1_*/);
    if (rc == -1) {
        cli->server_->DeleteClient(cli);//close the client
    }
}

void Server::ClientWriteCB(int fd, short what, void *arg)
{
        Client *cli = (Client *)arg;
        int rc = cli->Write();
        if (rc == -1) {
            cli->server_->DeleteClient(cli);//close the client
        } else if (rc == 2) {
            return ;
        }

        //到这里说明已经写完了
        //移除可写事件
#if 0
        Event e;
        e.fd_ = cli->fd();
        e.ptr_ = cli;
        event_.DelWriteEvent(e);
#endif
        event_free(cli->write_event());
        cli->set_write_event(NULL);
        
        return ;
}

void Server::SlaveReadCB(int fd, short what, void *arg)
{
    Slave *s = (Slave *)arg;
    int rc = s->Read();
    if (rc == 2) {
        return ;
    }

    //到这里表示读完了slave 发来的信息
    return ;
}
    
void Server::SlaveWriteCB(int fd, short what, void *arg)
{
    Slave *s = (Slave *)arg;
    int rc = s->Write();
    if (rc == 2) {
        return ;
    }

    //到这里表示写完了要发给slave 的信息
    event_free(s->write_event());
    s->set_write_event(NULL);

#if 0
    Event e;
    e.fd_ = slave_1_->link_->fd();
    e.ptr_ = slave_1_;
    event_.DelWriteEvent(e);
#endif

    return ;
}

