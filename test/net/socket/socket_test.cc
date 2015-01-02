// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)
//

#include <stdlib.h>
#include <iostream>

#include <event/event.h>
#include <net/socket.h>

using namespace std;

ldb::event::Loop loop;
ldb::net::Socket c(&loop);

void ReadHandler(void *owner, int event)
{
    if (event == ldb::net::Socket::kRead) {
        char buf[1024];
        int bytes = c.Read(buf, 1024);
        if (bytes > 0) {
            buf[bytes] = '\0';
            cout << "server response, bytes:" 
                << bytes << ", content: " 
                << buf << endl;
        } else {
            cout << "server failed" << endl;

            c.Close();
            exit(0);
        }
    }
}

int main()
{
    c.SetHandler((void *)&c, ReadHandler);
    bool ret = c.Connect("127.0.0.1", 9527);
    if (!ret) {
        cout << "connect failed" << endl;
        return -1;
    }

    int rc = c.Write((char *)"hello", 5);
    if (rc <= 0) {
        c.Close();
        cout << "send failed" << endl;
        return -1;
    }

    ret = loop.Run();
    if (!ret) {
        cout << "event loop run failed" << endl;
        return -1;
    }

    return 0;
}
