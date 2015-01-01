// acceptor.h (2014-12-24)
// WangPeng (1245268612@qq.com)

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <string>

#include <event/event.h>

namespace ldb {
namespace net {

class Socket;

class Acceptor {

    public:
        Acceptor(ldb::event::Loop *loop);
        ~Acceptor();

        int Listen(const std::string &ip, int port, int backlog);
        Socket *Accept();
        void Close();
        
        int fd() const;
    
        int SetNonBlock(); //if uses, after listen() call it 
        void SetReuseAddr(); 

    private:
        int fd_;
        std::string ip_;
        int port_;
        int backlog_;

        ldb::event::Loop *loop_;
        ldb::event::Event event_;

    private:
        Acceptor(const Acceptor &);
        void operator=(const Acceptor &);

        static void Notify(int fd, int events, void *arg);
        void Process(int fd, int events);
};

} /*namespace ldb*/
} /*namespace net*/

#endif

