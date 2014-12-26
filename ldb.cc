// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <iostream>
#include <sys/epoll.h>
#include <errno.h>

#include "server.h"
#include "command.h"
#include "client.h"
#include "sock.h"
#include "proc.h"

#define LDB_EVENT_MAX_COUNTS 1024

Server server(8899);
int ldb_sockfd = 0;          
int epfd = 0;
struct epoll_event ev, events[LDB_EVENT_MAX_COUNTS];

void ldb_process_events()
{
    int client_fd = 0;
    while ( 1 )
    {
        int nfds = epoll_wait(epfd, events, LDB_EVENT_MAX_COUNTS, -1);
        if (nfds == -1) {
            fprintf(stderr, "epoll_wait failed %s\n", strerror(errno));        
            break;
        }

        int i;
        for (i = 0; i < nfds; i++) 
        {
            if (events[i].data.fd < 0) continue;

            if (events[i].data.fd == ldb_sockfd) 
            { 
                client_fd = server.Accept(NULL, NULL);
                if (client_fd >= 0) 
                {
                    Client *client = new Client(client_fd);
                    server.AddClient(client);

                    fprintf(stderr, "create a client \n");

                    ldb_set_sock_no_block(client_fd);

                    ev.data.fd = client_fd;
                    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

                    continue;
                } else if (client_fd < 0) {
                    fprintf(stderr, "ldb_accept error %d\n", strerror(errno));
                    continue;
                }
            }

            Client *cli = server.FindClinet(events[i].data.fd);
            if (events[i].events & EPOLLIN) //有来自cleint的数据
            { 
                client_fd = events[i].data.fd; //得到有数据的sokcet
                Client *client = cli;
                int read_len;
read_again:
                read_len = read(client_fd, client->recv_, 2048);    
                if (read_len < 0) {
                    if (read_len == EINTR || read_len == EAGAIN) {
                        goto read_again;
                    }
#if 0
                    ldb_free_client(cli->value);
                    ldb_del_list_node(ldb_server.clients, cli);

                    close(client_fd);
                    events[i].data.fd = -1;
#endif
                    fprintf(stderr, "read err:%s\n", strerror(errno));
                    continue;
                } else if (read_len == 0) {

                    server.DeleteClient(cli->fd_);

                    close(client_fd);
                    events[i].data.fd = -1;
                    fprintf(stderr, "after a  client close \n");
                    continue;
                }

                process_client_info(client);
                continue;
            }
#if 0
            if (events[i].events & EPOLLERR) //有来自cleint出错 
            { 
                fprintf(stderr, "EPOLLERR error: %s \n", strerror(errno));

                ldb_free_client(cli->value);
                ldb_del_list_node(ldb_server.clients, cli);

                close(client_fd);
                events[i].data.fd = -1;
                continue;
            }

            if (events[i].events & EPOLLHUP) //有来自client关闭
            { 
                fprintf(stderr, "EPOLLHUP close \n");

                ldb_free_client(cli->value);
                ldb_del_list_node(ldb_server.clients, cli);

                close(client_fd);
                events[i].data.fd = -1;
                continue;
            }
#endif

        }
    }

    close(epfd);

    return ;
}


void ldb_init_server()
{

    server.CreateComTable();
    server.socket_.setNoblock();

    ldb_sockfd = server.fd(); 

    struct epoll_event ev ;

    epfd = epoll_create( LDB_EVENT_MAX_COUNTS );

    ev.data.fd = ldb_sockfd; 
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, ldb_sockfd, &ev); 
    if (ret == -1) {
        close(ldb_sockfd);
        close(epfd);
        printf("epoll_ctl failed %s\n", strerror(errno));
        return ;
    }

    //if (server.daemonize_ ) ldb_daemon();
}


int main(int argc, char **argv)
{

    fprintf(stderr, "before init server\n");
    ldb_init_server(); 
    fprintf(stderr, "after init serverd\n");

    ldb_process_events();

#if 0
#endif
    return 0;
}            
