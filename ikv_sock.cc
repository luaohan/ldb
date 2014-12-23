#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "ikv_sock.h"

void ikv_set_sock_default_opt(int s)
{
    //设置SO_REUSEADDR选项(服务器快速重起)
    int on = 1;                          
    int result = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if (result < 0) {
        close(s);
        return ;
    }

    on = 1;
    result = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on)); //keep alive in TCP connection

    //设置SO_LINGER选项(防范CLOSE_WAIT挂住所有套接字, 强行关闭)
    struct linger optval;
    optval.l_onoff = 1;
    optval.l_linger = 60;
    result = setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&optval, sizeof(struct linger));
    if ( result < 0 ) {
        close( s );
        return ;
    }

#if 0
    int len = 2 * 1024 * 1024;
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(len));
    setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(len));
#endif

    return;
}

void ikv_set_sock_no_delay(int s)
{
    int opt;
    socklen_t optlen;

    optlen = sizeof(opt);  
    if (getsockopt(s, IPPROTO_TCP, 1/*TCP_NODELAY*/, &opt, &optlen) == -1) {
        return;
    }

    if (opt == 1) {
        return;
    }
    opt = 1;
    setsockopt(s, IPPROTO_TCP, 1/*TCP_NODELAY*/, &opt, sizeof( opt ));

    return;
}

int ikv_set_sock_no_block(int s)
{
    int flags;

    if ((flags = fcntl(s, F_GETFL, NULL)) < 0) {
        return -1;
    }

    if (fcntl(s, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }

    return 0;
}

int ikv_create_tcp_server(const char *ip, int port)
{
    assert(ip != NULL);
    
    struct sockaddr_in addr;
    int s = socket( AF_INET, SOCK_STREAM, 0 );
    if ( s == -1 ) {
        return -1;
    }

    ikv_set_sock_default_opt(s);

    addr.sin_family = AF_INET;
    if (strcmp(ip, "0.0.0.0") == 0) {
        addr.sin_addr.s_addr = INADDR_ANY; //htonl(INADDR_ANY);
    } else {
        addr.sin_addr.s_addr = inet_addr( ip );
    }

    addr.sin_port = htons( port );

    if (bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1 ) {
        close( s);
        s = -1;
        return -1;
    }

    if (listen(s, IKV_MAX_LISTEN_QUEUE ) == -1 ) {
        close(s);
        s = -1;
        return -1;
    }

    return s;
}

int ikv_create_tcp_client(const char *ip, int port)
{
    struct sockaddr_in addr;
    int result;
    int on = 1;
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if ( fd == -1 ) {
        return -1;
    }

    result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
    if (result < 0 ) {
        close(fd);
        fd = -1;
        return -1;
    }

    on = 1;
    result = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)); //not use Nagle delay algorithm
    result = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*) & on, sizeof(on)); //keep alive in TCP connection

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    result = connect(fd, (struct sockaddr *) & addr, sizeof(struct sockaddr));
    if ( result != 0 ) {
        close(fd);
        fd = -1;
        return -1;
    }

    return fd;
}

int ikv_accept(int listen_fd, char *ip, int *port)
{
    int fd = -1;
    struct sockaddr_in remote;
    int len = 0;

ReAccept:
    len = sizeof( struct sockaddr_in );

    fd = accept(listen_fd, (struct sockaddr *)&remote, (socklen_t *)&len);
    if (fd == -1) {
        int err = errno;
        if ( err == EINTR || err == EAGAIN ) {
            goto ReAccept;
        }
#ifdef  EPROTO
        if (err == EPROTO || err == ECONNABORTED) {
            goto ReAccept;
        }
#else
        if (err == ECONNABORTED ) {
            goto ReAccept;
        }
#endif
        return -1;
    }

    ikv_set_sock_no_delay(fd);

    if ( port != NULL ) {
        *port = ntohs(remote.sin_port);
    }

    if ( ip != NULL ) {
        strcpy( ip, inet_ntoa(remote.sin_addr));
    }

    return fd;
}
