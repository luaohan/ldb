// ldb_server.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <errno.h>

#include "ldb.server.h"
#include "ikv_sock.h"

Server::Server(int server_port)
:fd_(-1), server_port_(server_port), daemonize_(false),
    configure_(NULL), logfile_(NULL)
{

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

void Server::Insert(const Slice& key, const Slice& value)
{
    db_->Put(write_options_, key, value);
}

void Server::Get(const Slice& key, std::string* value)
{
    db_->Get(read_options_, value);
}

void Server::Delete(const Slice& key)
{
    db_->Delete(key);
}

void Server::AddCommand(Command &com)
{
   commands_.push_back(com);
}

void Server::AddClient(Client &cli)
{
    clients_.push_back(cli);
}

void Server::DeleteClient(Client &cli)
{
    vector<Client>::iterator i;
    for (i = commands_.begin(); i != commands_.end(); i++)
    {
        if (*i == cli) {
            commands_.erase(i);
            break;
        }
    }
}

void Server::Listen()
{
    int fd = ikv_create_tcp_server("0.0.0.0", server_port_);
    if (fd == -1) {
        fprintf(stderr, "create tcp server:", strerror(errno));
        exit(0);
    }

    fd_ = fd;
}

int Server::Accept(char *client_ip, int *client_port)
{
    return ikv_accept(fd_, client_ip, client_port);
}
