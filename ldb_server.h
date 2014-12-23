// ldb_server.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_SERVER_H_
#define _LDB_SERVER_H_

#include <iostream>
#include <vector>
#include <leveldb/db.h>

#include "ldb_command.h"
#include "ldc_client.h"

//using namespace std;

class Server {

    public:
        Server(int server_port);
        ~Server();

        //level::DB *get_db() const { return db_; }
        
        void AddCommand(Command &com);
        void AddClient(Client &cli);
        void DeleteClient(Client &cli);
        
        void Listen();
        int Accept(char *client_ip, int *client_port); 

        void Insert(const Slice& key, const Slice& value);
        void Get(const Slice& key, std::string* value);
        void Delete(const Slice& key);

    private:
        int fd_ ;

        int server_port_;
        
        bool daemonize_;

        leveldb::Option options_;
        leveldb::WriteOptions write_options_;
        leveldb::ReadOptions read_options_;
        level::DB *db_;
        
        std::vector<Command> commands_;
        std::vector<Client> clients_;

        char *configure_;
        char *logfile_;

};


#endif
