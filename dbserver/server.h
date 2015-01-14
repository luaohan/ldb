// ldb_server.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_SERVER_H_
#define _LDB_SERVER_H_

#include <iostream>
#include <vector>
#include <leveldb/db.h>
#include <leveldb/slice.h>
#include <string>

#include "../net/e_epoll.h"
#include "../util/config.h"

class Client;
class Acceptor;

extern bool quit;

class Server {

    public:
        Server();
        ~Server();

        int Insert(const leveldb::Slice& key, const leveldb::Slice& value);
        int Get(const leveldb::Slice& key, std::string* value);
        int Delete(const leveldb::Slice& key);

        int Run(const char *config_file);

    private:
        int ProcessEvent();
        void ProcessReadEvent();
        void ProcessWriteEvent();
        void DeleteClient(Client *c);

        void AddClient(Client *cli);
        void DeleteClient(int fd);
        Client *FindClient(int fd);


    private:
        leveldb::Options options_;
        leveldb::WriteOptions write_options_;
        leveldb::ReadOptions read_options_;
        leveldb::DB *db_;
        
        std::vector<Client *> clients_;

    public:
        Epoll event_;
        Event fired_read_[1024];
        Event fired_write_[1024];

        Acceptor *socket_;

        Config config_;

};


#endif
