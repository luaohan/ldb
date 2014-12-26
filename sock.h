// ldb_sock.h (2014-11-20)
// WangPeng (1245268612@qq.com)

#ifndef _ldb_SOCK_H_
#define _ldb_SOCK_H_

#define LDB_MAX_LISTEN_QUEUE 512

void ldb_set_sock_default_opt(int s);

/* disable nagle on socket */ 
void ldb_set_sock_no_delay(int s);

int ldb_set_sock_no_block(int s);

int ldb_create_tcp_server(const char *ip, int port);

int ldb_create_tcp_client(const char *ip, int port);

int ldb_accept(int listen_fd, char *client_ip, int *client_port);

#endif
