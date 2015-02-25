// server.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_SERVER_H_
#define _LDB_SERVER_H_

#include <string>
#include <iostream>
#include <map>

#include <leveldb/db.h>
#include <event2/event.h>

class Client;
class Acceptor;
class Slave;

class Server {
public:
    Server();
    ~Server();

    int Insert(const std::string &key, const std::string &value);
    int Get(const std::string &key, std::string* value);
    int Delete(const std::string &key);

    int Run();
    void Stop();
    
    Client *FindClient(int fd);
    
    static void ClientReadWriteCB(int fd, short what, void *arg);
    static void SlaveReadWriteCB(int fd, short what, void *arg);
    
    bool SlaveIsWrinting();

    void DeleteClient(Client *c);

    struct event_base *base() const {return base_; }

private:
    void DeleteClient(int fd);
    void AddClient(Client *cli);

    void ConnectSlave();
    bool StartListen();

    static void ListenNotify(int fd, short what, void *arg);
    static void ClientReadCB(int fd, short what, void *arg);
    static void SlaveReadCB(int fd, short what, void *arg);
    static void ConnectSlaveCB(int fd, short what, void *arg);

    void AddSlaveOp();
    void DelSlaveOp();
    void CheckListen();

private:
    leveldb::Options options_;
    leveldb::WriteOptions write_options_;
    leveldb::ReadOptions read_options_;
    leveldb::DB *db_;
    
    std::map<int, Client *> clients_;
    int no_conn_slave_nums_;
    struct event_base *base_;
    Acceptor *acceptor_;
    std::vector<Slave *> slaves_;
    bool server_can_write_;
};


#endif
