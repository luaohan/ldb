// ldb_server.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_SERVER_H_
#define _LDB_SERVER_H_

#include <iostream>
#include <vector>
#include <leveldb/db.h>
#include <leveldb/slice.h>
#include <string>

#include "command.h"
#include "client.h"
#include "acceptor.h"
#include "e_epoll.h"

class Command;
class Client;

class Server {

    public:
        Server(int server_port);
        ~Server();

        Command *FindCommand(char *name);

        void CreateComTable();

        void AddClient(Client *cli);
        void DeleteClient(int fd);
        Client *FindClinet(int fd);
#if 0 
        void Listen();
        int Accept(char *client_ip, int *client_port); 
#endif
        void Insert(const leveldb::Slice& key, const leveldb::Slice& value);
        void Get(const leveldb::Slice& key, std::string* value);
        void Delete(const leveldb::Slice& key);

        //int fd() const { return fd_; }

    private:
        void AddCommand(Command &com);

    private:
        //int fd_ ;
        //int server_port_;
        
        Acceptor socket_;

        Epoll event_;

        leveldb::Options options_;
        leveldb::WriteOptions write_options_;
        leveldb::ReadOptions read_options_;
        leveldb::DB *db_;
        
        std::vector<Command> commands_;
        std::vector<Client *> clients_;

        char *configure_;
        char *logfile_;
        
        bool daemonize_;

};


#endif
