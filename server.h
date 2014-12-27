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
        Server();
        ~Server();

        Command *FindCommand(char *name);

        void AddClient(Client *cli);
        void DeleteClient(int fd);
        Client *FindClient(int fd);

        void Insert(const leveldb::Slice& key, const leveldb::Slice& value);
        void Get(const leveldb::Slice& key, std::string* value);
        void Delete(const leveldb::Slice& key);

        void Run();

    private:
        void AddCommand(Command &com);
        void CreateComTable();

    private:
        leveldb::Options options_;
        leveldb::WriteOptions write_options_;
        leveldb::ReadOptions read_options_;
        leveldb::DB *db_;
        
        std::vector<Command> commands_;
        std::vector<Client *> clients_;

        char *configure_;
        char *logfile_;
        
        bool daemonize_;

    public:
        Epoll event_;
        int fired_fd[1024];

        Acceptor socket_;

};


#endif
