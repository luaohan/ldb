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
    socket_(NULL), slave1_(NULL), slave2_(NULL), 
    slave_1_(NULL), server_can_write_(false), time_out_(2000) //2 S
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
        socket_->set_reuseAddr();
        if (socket_->Listen("0.0.0.0", config_.server_port_, backlog) < 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }
        
        slave1_ = new Socket;
        if (slave1_ == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }

        slave1_->set_noblock();
        slave_1_ = new Slave(slave1_, this);  //这里只先弄一个 slave
        if (slave_1_ == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }

        slave2_ = new Socket;
        if (slave2_ == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }

        slave2_->set_noblock();

    } else {
        socket_->set_reuseAddr();
        if (socket_->Listen(config_.slave_ip_.c_str(), config_.slave_port_, backlog) < 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }
    }

    socket_->set_noblock();

    Event e;
    e.fd_ = socket_->fd();
    e.ptr_ = socket_;
    event_.AddReadEvent(e);

    if (config_.master_server_) {
        ConnectSlave();
    } else {
        server_can_write_ = true;
        time_out_ = -1;
    }

    if ( config_.daemon_ ) {
        Daemon();
    }

    //fprintf(stderr, "init server success\n");

    while (!quit) {
        ProcessEvent();
    }

}

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

            link->set_noblock();

            Client *cli = new Client(this, link);
            AddClient(cli);
            
            Event e;
            e.fd_ = link->fd();
            e.ptr_ = cli;
            event_.AddReadEvent(e);
        
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

            //到这里表示写完了要发给slave 的信息
            Event e;
            e.fd_ = slave_1_->link_->fd();
            e.ptr_ = slave_1_;
            event_.DelWriteEvent(e);
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
        Event e;
        e.fd_ = cli->fd();
        e.ptr_ = cli;
        event_.DelWriteEvent(e);
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
        time_out_ = -1;
        Event e;
        e.fd_ = slave1_->fd();
        e.ptr_ = slave_1_;
        event_.AddReadEvent(e);
    }
}

void Server::DeleteClient(Client *c)
{
    Event e;
    e.fd_ = c->fd();
    event_.DelReadEvent(e);
    
    DeleteClient(c->fd());
    delete c;
}

void Server::ConnectSlave()
{
    int ret = 
        slave1_->Connect(config_.slave1_ip_.c_str(), config_.slave1_port_);
    if (ret == -1) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        //process after 2 second
        TimeEvent te(slave1_, (tv.tv_sec + 2) * 1000000 + tv.tv_usec, config_.slave1_ip_, config_.slave1_port_);
        time_event_.push_back(te);
    }
    
    ret = slave2_->Connect(config_.slave2_ip_.c_str(), config_.slave2_port_);
    if (ret == -1) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        //process after 2 second
        TimeEvent te(slave2_, (tv.tv_sec + 2) * 1000000 + tv.tv_usec, config_.slave2_ip_, config_.slave2_port_);
        time_event_.push_back(te);
    }

    if (time_event_.empty()) {
        server_can_write_ = true;
        time_out_ = -1;
        Event e;
        e.fd_ = slave1_->fd();
        e.ptr_ = slave_1_;
        event_.AddReadEvent(e);
    }
}
