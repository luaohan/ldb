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

#include "server.h"
#include "string_type.h"
#include "../net/acceptor.h"
#include "../util/daemon.h"
#include "../util/log.h"

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
        return -1;
}

int Server::Get(const leveldb::Slice& key, std::string* value)
{
    leveldb::Status status;
    status = db_->Get(read_options_, key, value);
    if (status.ok())
        return 0;
    else 
        return -1;
}

int Server::Delete(const leveldb::Slice& key)
{
    leveldb::Status status;
    status = db_->Delete(write_options_, key);
    if (status.ok())
        return 0;
    else 
        return -1;
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
        if ((*i)->link_->GetFd() == fd) {
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
        if ((*i)->link_->GetFd() == fd) {
            return (*i);
        }
    }
}

int Server::Run(const char *config_file)
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
    if (socket_->Listen("0.0.0.0", config_.server_port_, backlog) < 0) {
        return -1;
    }

    socket_->SetNoblock();
    
    event_.AddReadEvent(socket_->GetFd());

    if ( config_.daemon_ ) Daemon();
    
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

