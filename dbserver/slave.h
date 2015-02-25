// slave.h (2015-01-23)
// WangPeng (1245268612@qq.com)

#ifndef _SLAVE_H_
#define _SLAVE_H_

#include <vector>
#include <event2/event.h>
#include <util/protocol.h>

class Server;
class Client;
class Socket;

class Slave {
public:
    Slave(Server *server, const std::string &ip, int port)
        : server_(server),
        ip_(ip),
        port_(port),
        connecting_(false),
        sock_(NULL),
        data_pos_(0),
        write_pos_(0),
        data_one_(false),
        data_two_(false),
        flag_(false),
        writing_(false),
        time_event_(NULL)
        {}

    ~Slave() {
        if (sock_ != NULL) {
            delete sock_;
        }
        
        if (time_event_ != NULL) {
            event_free(time_event_);
        }
    }

    //ok, return 0
    //error, return -1
    //没写完,return 2
    int Read();
    int Write();

private:
    static void Notify(int event, void *data);
    void OnNotify(int event);

private:
    Server *server_;
    std::string ip_;
    int port_;
    bool connecting_;

    Socket *sock_;
    bool writing_;
    int data_pos_;
    int write_pos_;
    bool data_one_;
    bool data_two_;
    char recv_[PACKET_LEN_SLAVE_REPLAY];
    char client_flag_[sizeof(short)];
    bool flag_;

    struct event *time_event_;
};

#endif

