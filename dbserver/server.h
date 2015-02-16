// server.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_SERVER_H_
#define _LDB_SERVER_H_

#include <string>
#include <iostream>
#include <vector>

#include <leveldb/db.h>
#include <leveldb/slice.h>
#include <event2/event.h>

#include <net/socket.h>
#include <util/config.h>

class Client;
class Acceptor;
class Slave;

class Server {
public:
    Server(Config &config);
    ~Server();

    int Insert(const leveldb::Slice& key, const leveldb::Slice& value);
    int Get(const leveldb::Slice& key, std::string* value);
    int Delete(const leveldb::Slice& key);

    int Run();
    void Stop();
    
    Client *FindClient(int fd);
    
    static void ClientReadWriteCB(int fd, short what, void *arg);
    static void SlaveReadWriteCB(int fd, short what, void *arg);

private:
    void DeleteClient(Client *c);
    void DeleteClient(int fd);
    void AddClient(Client *cli);

    void ConnectSlave();

    static void ListenCB(int fd, short what, void *arg);
    static void ClientReadCB(int fd, short what, void *arg);
    static void SlaveReadCB(int fd, short what, void *arg);
    static void ConnectSlaveCB(int fd, short what, void *arg);

private:
    leveldb::Options options_;
    leveldb::WriteOptions write_options_;
    leveldb::ReadOptions read_options_;
    leveldb::DB *db_;
    
    std::vector<Client *> clients_;

    int no_conn_slave_nums_;
    
public:
    struct event_base *base_;

    Acceptor *socket_;
    std::vector<Slave *> slaves_;

    Config config_;

    bool server_can_write_;
};


#endif
