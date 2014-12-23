// ikv_sock.h (2014-11-20)
// WangPeng (1245268612@qq.com)

#ifndef _IKV_SOCK_H_
#define _IKV_SOCK_H_

#define IKV_MAX_LISTEN_QUEUE 512

void ikv_set_sock_default_opt(int s);

/* disable nagle on socket */ 
void ikv_set_sock_no_delay(int s);

int ikv_set_sock_no_block(int s);

int ikv_create_tcp_server(const char *ip, int port);

int ikv_create_tcp_client(const char *ip, int port);

int ikv_accept(int listen_fd, char *client_ip, int *client_port);

#endif
