#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define PORT 9527
#define MAXFDS 5000
#define EVENTSIZE 100

#define BUFFER "HTTP/1.1 200 OK\r\nContent-Length: 5\r\nConnection: close\r\nContent-Type: text/html\r\n\r\nHello"
#define BUFFER_SIZE 87

#define MAXFD 10240

typedef struct event_s event_t;
typedef void (*event_handler_t)(int, int, void *);

struct event_s {
    int fd;
    //void *data;
    event_handler_t handler;
};

int g_epfd;
event_t *g_events[MAXFD];

void set_non_blocking(int fd);

void on_client(int fd, int events, void *data);

void on_listen(int listen_fd, int which, void *data)
{
    int rc;
    int cli_fd;
    struct sockaddr_in cin;
    socklen_t sin_len = sizeof(struct sockaddr_in);

    cli_fd = accept(listen_fd, (struct sockaddr *)&cin, &sin_len);
    if (cli_fd == -1) {
        perror("accept failed");
        exit(-1);
    }

    set_non_blocking(cli_fd);

    event_t *e = (event_t *)malloc(sizeof(event_t));
    assert(e != NULL);
    e->fd = cli_fd;
    e->handler = on_client;

    struct epoll_event ev;
#if 0
    ev.data.ptr = (void *)e;
#endif
    g_events[cli_fd] = (void *)e;
    ev.data.fd = cli_fd;
    ev.events = EPOLLIN;
    rc = epoll_ctl(g_epfd, EPOLL_CTL_ADD, e->fd, &ev);
    if (rc == -1) {
        perror("add client fd to epoll failed");
        exit(-1);
    }

    printf("connect from %s, fd: %d\n", inet_ntoa(cin.sin_addr), cli_fd);
}

void on_client(int fd, int events, void *data)
{
    assert(data != NULL);

    int ret;
    struct epoll_event ev;
    char buffer[512];

    if (events & EPOLLIN) {
        ret = recv(fd, buffer, sizeof(buffer),0);
        //printf("read ret..........= %d\n",ret);

        //note:
        //most reset ptr to event_t *e, here is data
#if 0
        ev.data.ptr = data;
#endif
        //g_events[fd] = data;
        ev.data.fd = fd;
        ev.events = EPOLLOUT | EPOLLET;
        ret = epoll_ctl(g_epfd, EPOLL_CTL_MOD, fd, &ev);
        if (ret == -1) {
            perror("modify client event failed");
            exit(-1);
        }

    } else if (events & EPOLLOUT) {
        ret = send(fd, BUFFER, BUFFER_SIZE, 0);
        //printf("send ret...........= %d\n", ret);

        ret = epoll_ctl(g_epfd, EPOLL_CTL_DEL, fd, NULL);
        if (ret == -1) {
            perror("del client fd failed");
            exit(-1);
        }

        close(fd);
        fprintf(stdout, "close fd: %d\n", fd);

        event_t *e = (event_t *)data;
        free(e);

    } else {
        fprintf(stderr, "unknown event type\n");
    }
}


int main(int argc, char *argv[])
{
    int i;
    int rc;
    int fd;
    int opt = 1;

    int nfds;
    struct epoll_event ev;
    struct epoll_event events[EVENTSIZE];

    struct sockaddr_in sin;
    socklen_t sin_len = sizeof(struct sockaddr_in);

    //create epoll fd
    g_epfd = epoll_create(MAXFDS);
    if (g_epfd == -1) {
        perror("epoll fd create failed");
        return -1;
    }

    //create listen fd
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("create listen socket failed");
        return -1;
    }

    set_non_blocking(fd);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));

    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_port = htons((short)(PORT));
    sin.sin_addr.s_addr = INADDR_ANY;
    rc = bind(fd, (struct sockaddr *)&sin, sizeof(sin));
    if (rc == -1) {
        perror("bind failed");
        return -1;
    }

    rc = listen(fd, 32);
    if (rc == -1) {
        perror("listen failed");
        return -1;
    }

    event_t *e = (event_t *)malloc(sizeof(event_t));
    assert(e != NULL);
    e->fd = fd;
    e->handler = on_listen;

#if 0
    ev.data.ptr = e;
#endif
    g_events[fd] = e;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    rc = epoll_ctl(g_epfd, EPOLL_CTL_ADD, e->fd, &ev);
    if (rc == -1) {
        perror("add listen fd to epoll failed");
        return -1;
    }

    while (1) {
        nfds = epoll_wait(g_epfd, events, EVENTSIZE , -1);
        for (i = 0; i < nfds; i++) {
#if 0
            //data.ptr is event_t pointer
            event_t *e = (event_t *)events[i].data.ptr;

            //e->handler is on_listen or on_client
            e->handler(e->fd, events[i].events, (void *)e);
#endif
            int get_fd = events[i].data.fd;
            g_events[get_fd]->handler(get_fd, events[i].events, g_events[get_fd]);
        }
    }

    return 0;
}

void set_non_blocking(int fd)
{
    int opts;
    opts = fcntl(fd, F_GETFL);
    if (opts < 0) {
        fprintf(stderr, "fcntl failed\n");
        return;
    }

    opts = opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) < 0) {
        fprintf(stderr, "fcntl failed\n");
        return;
    }

    return;
}
