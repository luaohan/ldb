// slave.h (2015-01-23)
// WangPeng (1245268612@qq.com)

#ifndef _SLAVE_H_
#define _SLAVE_H_

#include <vector>
#include <util/protocol.h>

class Client;
class Socket;
class Server;

class Slave {
    public:
        Slave(Socket *link, Server *server, struct event *read_event
                , struct event *write_event):
            link_(link),
            server_(server),
            read_event_(read_event),
            write_event_(write_event),
            data_pos_(0),
            write_pos_(0),
            data_one_(false),
            data_two_(false),
            flag_(false){}

        ~Slave(){
            if (read_event_ != NULL) {
                event_free(read_event_);
            }

            if (write_event_ != NULL) {
                event_free(write_event_);
            }
        }

        //ok, return 0
        //error, return -1
        //没写完,return 2
        int Read();
        int Write();
        
        std::vector<Client *> clients_;
        Socket *link_;

        struct event *read_event() const {
            return read_event_;
        }

        struct event *write_event() const {
            return write_event_;
        }

        void set_read_event(struct event *e) {
            read_event_ = e;
        }

        void set_write_event(struct event *e) {
            write_event_ = e;
        }

    private:
        Server *server_;

        int data_pos_;
        int write_pos_;
        bool data_one_;
        bool data_two_;
        char recv_[PACKET_LEN_SLAVE_REPLAY];
        char client_flag_[sizeof(short)];
        bool flag_;

        struct event *read_event_;
        struct event *write_event_;

};




#endif
