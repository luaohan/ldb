// ldb_server.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
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
    socket_(NULL), base_(NULL), 
    signal_event_(NULL), 
    server_can_write_(false),
    no_conn_slave_nums(0)
{

}

Server::~Server()
{
    if (socket_ != NULL) {
        delete socket_;
    }
    
    if (signal_event_ != NULL) {
        event_free(signal_event_);
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
    std::string file = config_file;

    int ret = config_.LoadConfig(file);
    if (ret != 0) {
        fprintf(stderr, "configfile is wrong.\n");
        return -1;
    }

    log = new Log(config_.log_file_, config_.level_, 0);
    if (log == NULL) {
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
    socket_->SetReuseAddr();
    if (socket_->Listen(config_.server_ip_.c_str(), config_.server_port_, backlog) < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    socket_->SetNonBlock();
     
    if (config_.master_server_) {
        std::vector<ConfigSlave> slaves = config_.slaves_;
        std::vector<ConfigSlave>::iterator i = slaves.begin();
        for (; i != slaves.end(); i++) {
            std::string ip = (*i).ip_;
            int port = (*i).port_;
            Socket *socket = new Socket(ip.c_str(), port);
            //socket->SetNonBlock();
            Slave *slave = new Slave(socket, this);
            slaves_.push_back(slave);
        }
    } 
    
    base_ = event_base_new();
    struct event *e = event_new(base_, socket_->fd(),
                EV_READ | EV_PERSIST, Server::ListenCB, this);
    event_add(e, NULL);
    socket_->set_event(e);

    if (config_.master_server_) {
        ConnectSlave();
    } else {
        server_can_write_ = true;
    }

    if ( config_.daemon_ ) {
        Daemon();
    }

    //fprintf(stderr, "init server success\n");

    event_base_dispatch(base_);

    return 0;
}

void Server::DeleteClient(Client *c)
{
    DeleteClient(c->fd());
    
    delete c;
}

void Server::ConnectSlave()
{
    int ret;
    std::vector<Slave *>::iterator i = slaves_.begin();

    for (; i != slaves_.end(); i++) {
        Slave *slave = *i;
        ret = slave->link_->Connect();
        if (ret == -1) { 
            fprintf(stderr, "error connect %s:%d \n", slave->link_->ip(), slave->link_->port());
            //没有连接成功，加一个时间事件，1 S 发一次连接
            struct event *e = 
                event_new(slave->server_->base_, -1, EV_PERSIST, Server::ConnectSlaveCB, slave);
            struct timeval t = {2, 0};
            event_add(e, &t);
            slave->set_time_event(e);
    
            slave->server_->no_conn_slave_nums++;
            continue;
        }

        //连接成功，添加读事件
        int fd = slave->link_->fd();
        struct event *e = event_new(slave->server_->base_, fd, EV_READ | EV_PERSIST,
                Server::SlaveReadCB, slave);
        event_add(e, NULL);
        slave->link_->set_event(e);
    }
    
    i = slaves_.begin();
    if ((*i)->server_->no_conn_slave_nums == 0) {
        (*i)->server_->server_can_write_ = true;
    }

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
    
    Client *cli = new Client(server, link, server->slaves_);
    server->AddClient(cli);
    
    struct event *e = event_new(server->base_, link->fd(), EV_READ | EV_PERSIST,
                Server::ClientReadCB, cli);
    event_add(e, NULL);

    link->set_event(e);

    log_info("---<create a client:[ip:%s],[port:%d],[fd:%d]>---", 
            link->ip(), link->port(), link->fd());
    
    printf("a new connect\n");

    return ;
}

void Server::ClientReadCB(int fd, short what, void *arg)
{
    Client *cli = (Client *)arg;
    int rc = cli->Read();
    if (rc == -1) {
        cli->server_->DeleteClient(cli);//close the client
    }
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
    

void Server::ConnectSlaveCB(int fd, short what, void *arg)
{
    Slave *slave = (Slave *)arg;
    int ret = slave->link_->Connect();
    fprintf(stderr, "connect %s:%d \n", slave->link_->ip(), slave->link_->port());
    if (ret == -1) { //没有连接成功，依然 1 S 发一次连接
        return ;
    }

    //连接成功，
    //移除时间事件
    event_free(slave->time_event());
    slave->set_time_event(NULL);
    //添加读事件
    int fds = slave->link_->fd();
    struct event *e = event_new(slave->server_->base_, fds, EV_READ | EV_PERSIST,
            Server::SlaveReadCB, slave);
    event_add(e, NULL);
    slave->link_->set_event(e);

    slave->server_->no_conn_slave_nums--;

    if (slave->server_->no_conn_slave_nums == 0) {
        slave->server_->server_can_write_ = true;
    }
}

void Server::ClientReadWriteCB(int fd, short what, void *arg)
{
    if (what & EV_READ) {
        ClientReadCB(fd, what, arg);
    }
   
    if (what & EV_WRITE) {
        Client *cli = (Client *)arg;
        int rc = cli->Write();
        if (rc == -1) {
            cli->server_->DeleteClient(cli);//close the client
        } else if (rc == 2) {
            return ;
        }

        //到这里说明已经写完了
        //移除可写事件
        //先将原来的事件删除，再添加读事件
        event_free(cli->link_->event());

        int fds = cli->fd();
        struct event *e = event_new(cli->server_->base_, fds, EV_READ | EV_PERSIST, 
                Server::ClientReadCB, cli);
        event_add(e, NULL);
        cli->link_->set_event(e);
    }
    
    return ;
}

void Server::SlaveReadWriteCB(int fd, short what, void *arg)
{
    if (what & EV_READ) {
        SlaveReadCB(fd, what, arg);
    }

    if (what & EV_WRITE) {
    
        Slave *s = (Slave *)arg;
        int rc = s->Write();
        if (rc == 2) {
            return ;
        }

        //到这里表示写完了要发给slave 的信息
        //移除写事件
        event_free(s->link_->event());

        //添加读事件
        int fds = s->link_->fd();
        struct event *e = event_new(s->server_->base_, fds, EV_READ | EV_PERSIST,
                Server::SlaveReadCB, s);
        event_add(e, NULL);
        s->link_->set_event(e);
    }

    return ;
}
