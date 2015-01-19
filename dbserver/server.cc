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

#include <net/acceptor.h>
#include <util/daemon.h>
#include <util/log.h>

#include <dbserver/server.h>
#include <dbserver/client.h>

bool quit = false;

void SigProcess();
static void SigtermHandler(int sig);

Server::Server():
    socket_(NULL)
{

}

Server::~Server()
{
    if (socket_ != NULL) {
        delete socket_;
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

int Server::Run(const char *config_file, const char *ip, int port)
{
    SigProcess();

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

    socket_ = new Acceptor;
    if (socket_ == NULL) {
        return -1;
    }

    int backlog = 512;
    if (ip != NULL && port != 0) {
        if (socket_->Listen(ip, port, backlog) < 0) {
            return -1;
        }
    } else if (ip != NULL && port == 0) {
        if (socket_->Listen(ip, config_.server_port_, backlog) < 0) {
            return -1;
        }
    } else if (ip == NULL && port != 0) {
        if (socket_->Listen("0.0.0.0", port, backlog) < 0) {
            return -1;
        }
    } else if (ip == NULL && port == 0) {
        if (socket_->Listen("0.0.0.0", config_.server_port_, backlog) < 0) {
            return -1;
        }
    }

    socket_->set_noblock();

    Event e;
    e.fd_ = socket_->fd();
    e.ptr_ = socket_;
    event_.AddReadEvent(e);

    if ( config_.daemon_ ) {
        Daemon();
    }

    //fprintf(stderr, "init server success\n");

    while (!quit) {
        ProcessEvent();
    }

    if (log != NULL) {
        delete log;
    }

    
    return 0;
}

void SigProcess()
{
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = SigtermHandler;
    sigaction(SIGTERM, &act, NULL);
}

static void SigtermHandler(int sig)
{
    quit = true;
    log_info("server exit");
}


int Server::ProcessEvent()
{
    int n = event_.WaitReadEvent(fired_read_, fired_write_);
    if ( n < 0) {
        log_error("WaitReadEvent error:[%s]", strerror(errno));
        return -1;
    }
    
    //读事件
    ProcessReadEvent();
    
    //写事件
    ProcessWriteEvent();

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

        Client *cli = (Client *)fired_read_[i].ptr_;
        int rc = cli->Read();
        if (rc == -1) {
            DeleteClient(cli);//close the client
        }
    }
}

void Server::ProcessWriteEvent()
{
    for (int i = 0; fired_write_[i].ptr_ != NULL; i++) 
    {
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

void Server::DeleteClient(Client *c)
{
    Event e;
    e.fd_ = c->fd();
    event_.DelReadEvent(e);
    
    DeleteClient(c->fd());
    delete c;
}
