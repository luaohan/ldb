// server.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_SERVER_H_
#define _LDB_SERVER_H_

#include <iostream>
#include <vector>
#include <leveldb/db.h>
#include <string>

#include <net/acceptor.h>
#include <event/event.h>

#include "command.h"
#include "client.h"
#include "config.h"

namespace ldb {
namespace dbserver {

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

        //ok: return 0
        //error: return -1
        int Insert(const std::string& key, const std::string& value);
        int Get(const std::string& key, std::string* value);
        int Delete(const std::string& key);

        int Run(const char *config_file);

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

    public:
        ldb::event::Event event_;
        int fired_fd[1024];

        ldb::net::Acceptor acceptor_;

        Config config_;
};

} /*namespace dbserver*/
} /*namespace ldb*/


#endif
