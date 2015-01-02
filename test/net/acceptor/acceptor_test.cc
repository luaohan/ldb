// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)
//

#include <iostream>

#include <net/acceptor.h>
#include <net/socket.h>

using namespace std;
using namespace ldb::event;
using namespace ldb::net;

static void OnRead(void *owner, int event) {
    cout << "enter onread callback" << endl;

    Socket *s = (Socket *)owner;
    char buf[1024];
    int bytes = s->Read(buf, 1024);
    if (bytes <= 0) {
        delete s;
        return;
    }

    s->Write(buf, bytes);
}

class Echo {
public:
    Echo() : acceptor_(&loop_) {}
    ~Echo() {};

    bool Run() {
        acceptor_.SetHandler(this, OnConnect);
        
        if (!acceptor_.Listen("0.0.0.0", 9527, 5)) {
            cout << "listen on 9527 failed" << endl;
            return false;
        }

        if (!loop_.Run()) {
            cout << "event loop run failed" << endl;
            return false;
        }

        return true;
    }

private:
    static void OnConnect(void *owner, Socket *s) {
        cout << "enter OnConnect" << endl;
        Echo *echo = (Echo *)owner;
        echo->Process(s);
    }

    void Process(Socket *s) {
        s->SetHandler((void *)s, OnRead);
    }


private:
    ldb::event::Loop loop_;
    Acceptor acceptor_;
};


int main()
{
    Echo echo;
    echo.Run();

    return 0;
}
