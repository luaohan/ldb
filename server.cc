// ldb_server.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <errno.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#include "server.h"
//#include "sock.h"
#include "string_type.h"

Server::Server(int server_port): fd_(-1), server_port_(server_port), 
    daemonize_(false), configure_(NULL), logfile_(NULL)
{
    options_.create_if_missing = true;

    leveldb::Status status 
        = leveldb::DB::Open(options_, "/tmp/test_ldb", &db_);
    assert(status.ok());
}

Server::~Server()
{
    configure_ = NULL;
    logfile_ = NULL;
    close(fd_);
    fd_ = -1;
}

void Server::Insert(const leveldb::Slice& key, const leveldb::Slice& value)
{
    db_->Put(write_options_, key, value);
}

void Server::Get(const leveldb::Slice& key, std::string* value)
{
    db_->Get(read_options_, key, value);
}

void Server::Delete(const leveldb::Slice& key)
{
    db_->Delete(write_options_, key);
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
        if ((*i)->fd_ == fd) {
            clients_.erase(i);
            break;
        }
    }
}

Client *Server::FindClinet(int fd)
{
    std::vector<Client *>::iterator i;
    for (i = clients_.begin(); i != clients_.end(); i++)
    {
        if ((*i)->fd_ == fd) {
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

#if 0
void Server::Listen()
{
    int fd = ldb_create_tcp_server("0.0.0.0", server_port_);
    if (fd == -1) {
        fprintf(stderr, "create tcp server:", strerror(errno));
        exit(0);
    }

    fd_ = fd;
}

int Server::Accept(char *client_ip, int *client_port)
{
    return ldb_accept(fd_, client_ip, client_port);
}
#endif

void Server::CreateComTable()
{
    Command ldb_commands_table[] = {
        {"set", ldb_set_command, 3, "w"}/*,
        {"get", ldb_get_command, 2, "r"},
        {"update", ldb_update_command, 3, "w"},
        {"del", ldb_del_command, 2, "w"},
        {"lookall", ldb_lookall_command, 1, "r"},
        {"clear", ldb_clear_command, 1, "w"},
        {"select", ldb_select_command, 2, "w"}*/
    };  
    
    AddCommand(ldb_commands_table[0]);
#if 0
    server.AddCommand(ldb_commands_table[1]);
    server.AddCommand(ldb_commands_table[2]);
    server.AddCommand(ldb_commands_table[3]);
    server.AddCommand(ldb_commands_table[4]);
    server.AddCommand(ldb_commands_table[5]);
    server.AddCommand(ldb_commands_table[6]);
#endif

}
