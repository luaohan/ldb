// server.cc (2014-12-23)
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
#include <util/log.h>

#include <dbserver/server.h>
#include <dbserver/client.h>
#include <dbserver/slave.h>
#include <dbserver/signal.h>

extern Config config;

bool quit = false;

Server::Server()
    : acceptor_(NULL),
    base_(event_base_new()), 
    no_conn_slave_nums_(0)
{
}

Server::~Server()
{
    if (acceptor_ != NULL) {
        delete acceptor_;
    }

    std::vector<Slave *>::iterator i = slaves_.begin();
    for (; i != slaves_.end(); i++) {
        delete (*i);
    }

    if (base_ != NULL) {
        event_base_free(base_);
    }
    
    delete db_;
}

int Server::Insert(const std::string& key, const leveldb::Slice& value)
{
    leveldb::Status status;
    status = db_->Put(write_options_, key, value);
    if (status.ok())
        return 0;
    else { 
        log_error("insert error: %s\n", status.ToString().c_str());
        return -1; //一般不会发生
    }
}

int Server::Get(const std::string& key, std::string* value)
{
    leveldb::Status status;
    status = db_->Get(read_options_, key, value);
    if (status.ok())
        return 0;
    else 
        return -1;  //没有这个 key
}

int Server::Delete(const std::string& key)
{
    leveldb::Status status;
    status = db_->Delete(write_options_, key);
    if (status.ok())
        return 0;
    else { 
        log_error("delete error: %s\n", status.ToString().c_str());
        return -1; //一般不会发生
    }
}

void Server::DeleteClient(int fd)
{
    std::map<int, Client *>::iterator pos
        = clients_.find(fd);
    if (pos == clients_.end()) {
        //log
    }

    clients_.erase(pos);
}

Client *Server::FindClient(int fd)
{
    std::map<int, Client *>::iterator pos
        = clients_.find(fd);
    if (pos == clients_.end()) {
        return NULL;
    }

    return *pos;
}

int Server::Run()
{
    log = new Log(config.log_file_, config.level_, 0);
    assert(log != NULL);
    
    options_.create_if_missing = true;
    leveldb::Status status 
        = leveldb::DB::Open(options_, config.db_directory_.c_str(), &db_);
    if (!status.ok()) {
        log_error("open db error: %s", status.ToString().c_str());
        return -1;
    }
    
    if (config.master_server_) {
        ConnectSlave();
    }
    
    event_base_dispatch(base_);

    return 0;
}

void Server::Stop()
{
    event_base_loopexit(base_, NULL);
}

void Server::DeleteClient(Client *c)
{
    DeleteClient(c->fd());
    
    delete c;
}

void Server::ConnectSlave()
{
    std::vector<ConfigSlave>::iterator i = config.slaves.begin();
    for (; i != config.slaves.end(); i++) {
        std::string ip = (*i).ip_;
        int port = (*i).port_;
        Slave *slave = new Slave(this, ip, port);
        slaves_.push_back(slave);

        ret = slave->Connect();
        if (ret == -1) { 
            log_warn("error connect %s:%d \n", 
                    sock->ip().c_str(), sock->port());
            AddSlaveOp(); 
            continue;
        }
    }

    CheckListen();
}

void Server::ListenNotify(void *arg)
{
    Server *server = (Server *)arg;
    server->Accept(); 
    return ;
}

void Server::Accept()
{
    AsyncSocket *s = acceptor_->Accept();
    if (s == NULL) {
        log_error("Accept error:[%s]", strerror(errno));
        return ;
    }
    
    Client *cli = new Client(this, s);
    assert(cli != NULL);
    clients_.insert(std::make_pair(s->fd, cli));

    log_info("---<create a client:[ip:%s],[port:%d],[fd:%d]>---", 
            link->ip(), link->port(), link->fd());
}

void Server::ClientReadCB(int fd, short what, void *arg)
{
    Client *cli = (Client *)arg;
    int rc = cli->Read();
    if (rc == -1) {
        log_info("delete cli\n");
        
        if (cli->server_->SlaveIsWrinting()) {
            cli->exit_ = true;  //不要立即删除cli
            return ;
        }
        
        cli->server_->DeleteClient(cli);//close the client
    }
}

bool Server::SlaveIsWrinting()
{
    std::vector<Slave *>::iterator i = slaves_.begin();
    for (; i != slaves_.end(); i++) {
        if ((*i)->writing_) {
            return true;
        }
    }

    return false;
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
    
void Server::ClientReadWriteCB(int fd, short what, void *arg)
{
    if (what & EV_READ) {
        ClientReadCB(fd, what, arg);
        return ;
    }
   
    if (what & EV_WRITE) {
        Client *cli = (Client *)arg;
        int rc = cli->Write();
        if (rc == -1) {
            log_info("delete cli\n");
            
            if(cli->server_->SlaveIsWrinting()) {
                cli->exit_ = true;  //不要立即删除cli
                return ;
            }
            
            cli->server_->DeleteClient(cli);//close the client
            return ;
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
        return ;
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

bool Server::StartListen()
{
    acceptor_ = new Acceptor(base_, ListenNotify, this);
    assert(acceptor_ != NULL);

    int rc = acceptor_->Listen(config.server_ip_, config.server_port_)
    if (rc < 0) {
        log_error("%s", strerror(errno));
        return false;
    }

    return true;
}

void Server::AddSlaveOp()
{
    no_conn_slave_nums_++;
}

void Server::DelSlaveOp()
{
    no_conn_slave_nums_;

    CheckListen();
}

void Server::CheckListen()
{
    if (no_conn_slave_nums_ <= 0) {
        if (!StartListen()) {
            //log
            exit(1);
        }
    }
}

