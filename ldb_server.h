// ldb_server.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_SERVER_H_
#define _LDB_SERVER_H_

#include <iostream>
#include <vector>
#include <leveldb/db.h>
#include <leveldb/slice.h>

#include "ldb_command.h"
#include "ldb_client.h"

class Command;
class Client;

class Server {

    public:
        Server(int server_port);
        ~Server();

        //level::DB *get_db() const { return db_; }
        
        void AddCommand(Command &com);
        Command *FindCommand(char *name);

        void AddClient(Client *cli);
        void DeleteClient(int fd);
        Client *FindClinet(int fd);
        
        void Listen();
        int Accept(char *client_ip, int *client_port); 

        void Insert(const leveldb::Slice& key, const leveldb::Slice& value);
        void Get(const leveldb::Slice& key, std::string* value);
        void Delete(const leveldb::Slice& key);

        int fd() const { return fd_; }

    private:
        int fd_ ;

        int server_port_;
        
        bool daemonize_;

        leveldb::Options options_;
        leveldb::WriteOptions write_options_;
        leveldb::ReadOptions read_options_;
        leveldb::DB *db_;
        
        std::vector<Command> commands_;
        std::vector<Client *> clients_;

        char *configure_;
        char *logfile_;

};


#endif
