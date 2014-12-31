// server.cc (2014-12-23)
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

#include <util/daemon.h>
#include <util/log.h>

#include "server.h"
#include "string_type.h"

namespace ldb {
namespace dbserver {

ldb::util::Log *log;

Server::Server()
{
}

Server::~Server()
{
}

int Server::Insert(const std::string& key, const std::string& value)
{
    leveldb::Status status;
    leveldb::Slice slicek(key);
    leveldb::Slice slicev(value);

    status = db_->Put(write_options_, slicek, slicev);
    if (status.ok())
        return 0;
    else 
        return -1;
}

int Server::Get(const std::string& key, std::string* value)
{
    leveldb::Status status;
    leveldb::Slice slicek(key);
    status = db_->Get(read_options_, slicek, value);
    if (status.ok())
        return 0;
    else 
        return -1;
}

int Server::Delete(const std::string& key)
{
    leveldb::Status status;
    leveldb::Slice slicek(key);
    status = db_->Delete(write_options_, slicek);
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
        if ((*i)->link_->fd() == fd) {
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
        if ((*i)->link_->fd() == fd) {
            return (*i);
        }
    }

    return NULL;
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
    Command commands_table[] = {
        {"set", set_command, 3, "w"},
        {"get", get_command, 2, "r"},
        {"del", del_command, 2, "w"}/*,
        {"update", update_command, 3, "w"},
        {"lookall", lookall_command, 1, "r"},
        {"clear", clear_command, 1, "w"},
        {"select", select_command, 2, "w"}*/
    };  
    
    AddCommand(commands_table[0]);
    AddCommand(commands_table[1]);
    AddCommand(commands_table[2]);
#if 0
    AddCommand(commands_table[3]);
    AddCommand(commands_table[4]);
    AddCommand(commands_table[5]);
    AddCommand(commands_table[6]);
#endif

}

int Server::Run(const char *config_file)
{
    int ret = config_.LoadConfig(config_file);
    if (ret != 0) {
        fprintf(stderr, "configfile is wrong.\n");
        return -1;
    }

    log = new ldb::util::Log(config_.log_file_, config_.level_);

    options_.create_if_missing = true;
    leveldb::Status status 
        = leveldb::DB::Open(options_, config_.db_directory_.c_str(), &db_);
    assert(status.ok());

    CreateComTable();                                          

    int backlog = 512;
    int rc = acceptor_.Listen("0.0.0.0", config_.server_port_, backlog);
    if (rc == -1) {
        return -1;
    }
    //must be after listen
    acceptor_.SetNonBlock();

    event_.AddEvent(acceptor_.fd(), ldb::event::Event::E_EPOLLIN);

    if ( config_.daemon_ )  {
        ldb::util::Daemon();
    }

    return 0;
}

} /*namespace dbserver*/
} /*namespace ldb*/

