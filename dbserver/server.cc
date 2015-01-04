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

#include "server.h"
#include "string_type.h"
#include "../util/daemon.h"
#include "../util/log.h"

Server::Server()
{
}

Server::~Server()
{
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

void Server::AddCommand(Command &com)
{
   commands_.push_back(com);
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
        if ((*i)->link_->getFd() == fd) {
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
        if ((*i)->link_->getFd() == fd) {
            return (*i);
        }
    }
}

Command *Server::FindCommand(char *name)
{
    std::vector<Command>::iterator i;
    for (i = commands_.begin(); i != commands_.end(); i++)
    {
        if (strcmp(i->name, name) == 0 ) {
            return &(*i);
        }
    }

    return NULL;
}

void Server::CreateComTable()
{
    Command ldb_commands_table[] = {
        {"set", ldb_set_command, 3, "w"},
        {"get", ldb_get_command, 2, "r"},
        {"del", ldb_del_command, 2, "w"}/*,
        {"update", ldb_update_command, 3, "w"},
        {"lookall", ldb_lookall_command, 1, "r"},
        {"clear", ldb_clear_command, 1, "w"},
        {"select", ldb_select_command, 2, "w"}*/
    };  
    
    AddCommand(ldb_commands_table[0]);
    AddCommand(ldb_commands_table[1]);
    AddCommand(ldb_commands_table[2]);
#if 0
    AddCommand(ldb_commands_table[3]);
    AddCommand(ldb_commands_table[4]);
    AddCommand(ldb_commands_table[5]);
    AddCommand(ldb_commands_table[6]);
#endif

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

    CreateComTable();                                          

    socket_.setNoNagle();

    int backlog = 512;
    if (socket_.Listen("0.0.0.0", config_.server_port_, backlog) < 0) {
        return -1;
    }

    socket_.setNoblock();
    
    event_.addReadEvent(socket_.getFd());

    if ( config_.daemon_ ) ldb_daemon();
    
    return 0;
}
