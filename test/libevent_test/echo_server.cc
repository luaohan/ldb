
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <event2/event.h>
#include <net/acceptor.h>
#include <net/socket.h>

    
struct event_base *base;
Acceptor server_socket;
struct event *events[10240];

void client_cb(int fd, short what, void *arg);

void listen_cb(int fd, short what, void *arg)
{
    fprintf(stderr, "listen_cb arg: %d \n", *((int *)arg));

    Socket *socket = server_socket.Accept();
    if (socket == NULL) {
        fprintf(stderr, "error: accept()\n");
        return ;
    }
    socket->SetNonBlock();

    struct event* event = event_new(base, socket->fd(), EV_READ, 
            client_cb, socket);
    if (event == NULL) {
        fprintf(stderr, "error: event_new()\n");
        return ;
    }
    events[socket->fd()] = event;

    int ret = event_add(event, NULL);
    if (ret == -1) {
        fprintf(stderr, "error: event_add()\n");
        return ;
    }

    return ;
}

void client_cb(int fd, short what, void *arg)
{
    Socket *socket = (Socket *)arg;
    char recv_buf[1024];
    int ret = socket->ReadData(recv_buf, 1024);
    recv_buf[ret] = '\0';

    fprintf(stderr, "recv form fd: %d, info: %s\n", socket->fd(), recv_buf);
    char str[12]= "hello world";
    ret = socket->WriteData(str, 11);
    
    event_free(events[socket->fd()]);

    delete socket;

    return ;
}

int main()
{
    base = event_base_new();
    if (base == NULL) {
        fprintf(stderr, "error: event_base_new()\n");
        return -1;
    }
    
    server_socket.SetReuseAddr();
    int ret = server_socket.Listen("0.0.0.0", 8899, 10);
    if (ret == -1) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return -1;
    }
    server_socket.SetNonBlock();

    int num = 10;
    struct event* event = event_new(base, server_socket.fd(),
                EV_READ | EV_PERSIST , listen_cb, &num);
    if (event == NULL) {
        fprintf(stderr, "error: event_new()\n");
        return -1;
    }

    num = 500;

    ret = event_add(event, NULL);
    if (ret == -1) {
        fprintf(stderr, "error: event_add()\n");
        return -1;
    }

    event_base_dispatch(base);
   
    event_free(event);
    event_base_free(base);

    return 0;
}
