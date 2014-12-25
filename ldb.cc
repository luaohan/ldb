// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <iostream>
#include <sys/epoll.h>
#include <errno.h>

#include "server.h"
#include "command.h"
#include "client.h"
#include "string_type.h"
#include "str.h"
#include "sock.h"

#define LDB_EVENT_MAX_COUNTS 1024
#define LDB_SPACE "                                          "
#define LDB_NO_THE_COMMAND "no have the command"

Server server(8899);
int ikv_sockfd = 0;          
int epfd = 0;
struct epoll_event ev, events[LDB_EVENT_MAX_COUNTS];

Command ldb_commands_table[] = {
    {"set", ikv_set_command, 3, "w"}/*,
    {"get", ikv_get_command, 2, "r"},
    {"update", ikv_update_command, 3, "w"},
    {"del", ikv_del_command, 2, "w"},
    {"lookall", ikv_lookall_command, 1, "r"},
    {"clear", ikv_clear_command, 1, "w"},
    {"select", ikv_select_command, 2, "w"}*/
};  

void ldb_create_commands_table()
{
    server.AddCommand(ldb_commands_table[0]);
#if 0
    server.AddCommand(ldb_commands_table[1]);
    server.AddCommand(ldb_commands_table[2]);
    server.AddCommand(ldb_commands_table[3]);
    server.AddCommand(ldb_commands_table[4]);
    server.AddCommand(ldb_commands_table[5]);
    server.AddCommand(ldb_commands_table[6]);
#endif

}

int ikv_write(int fd, void *buf, int len)   
{
    int write_len;

write_again:
    write_len = write(fd, buf, len);
    if (write_len < 0)  
    {   
        if (write_len == EINTR) {
            goto write_again;
        }   

        return write_len;
    }   

    return write_len;
}

int ikv_tell_client( Client *client )
{
    return ikv_write(client->fd_, client->replay_, strlen(client->replay_));
}  

void process_client_info(Client *client)
{
    fprintf(stderr, "client_fd:%d, data:|%s|\n", client->fd_, client->recv_);

    str2lower(client->recv_);
    strs2tokens(client->recv_, LDB_SPACE, client->argv_, &client->argc_);

    int tell_len;                                                            
    Command *command = server.FindCommand(client->argv_[0]);
    if (command == NULL) 
    {
        memcpy(client->replay_, LDB_NO_THE_COMMAND, strlen(LDB_NO_THE_COMMAND));

        tell_len = ikv_tell_client(client);
        if (tell_len < 0) {
            memcpy(client->replay_, 0, 2048);
            fprintf(stderr, "ikv_tell_client error:%s\n", strerror(errno));
            return ;
        }

        memset(client->recv_, 0, 2048);
        memset(client->replay_, 0, 2048);
        return ;
    }

    client->cmd = command;
    command->proc(&server, client);

    tell_len = ikv_tell_client(client);
    if (tell_len < 0) {
        fprintf(stderr, "ikv_tell_client error:%s\n", strerror(errno));
        return ;
    }


    memset(client->recv_, 0, 2048);
    memset(client->replay_, 0, 2048);

    return ;
}



void ikv_process_events()
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

            if (events[i].data.fd == ikv_sockfd) 
            { 
                client_fd = server.Accept(NULL, NULL);
                if (client_fd >= 0) 
                {
                    Client *client = new Client(client_fd);
                    server.AddClient(client);

                    fprintf(stderr, "create a client \n");

                    ikv_set_sock_no_block(client_fd);

                    ev.data.fd = client_fd;
                    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

                    continue;
                } else if (client_fd < 0) {
                    fprintf(stderr, "ikv_accept error %d\n", strerror(errno));
                    continue;
                }
            }

#if 0
            ikv_list_node_t *cli = ikv_server.clients->head;
            while( cli ) 
            {
                if ( ((ikv_client_t *)(cli->value))->fd != events[i].data.fd ) {
                    cli = cli->next; 
                    continue;
                }

                break;
            }
#endif        
            Client *cli = server.FindClinet(events[i].data.fd);
            if (events[i].events & EPOLLIN) //有来自cleint的数据
            { 
                client_fd = events[i].data.fd; //得到有数据的sokcet
                //ikv_client_t *client = (ikv_client_t *)(cli->value);
                Client *client = cli;
                int read_len;
read_again:
                read_len = read(client_fd, client->recv_, 2048);    
                if (read_len < 0) {
                    if (read_len == EINTR || read_len == EAGAIN) {
                        goto read_again;
                    }
#if 0
                    ikv_free_client(cli->value);
                    ikv_del_list_node(ikv_server.clients, cli);

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

                ikv_free_client(cli->value);
                ikv_del_list_node(ikv_server.clients, cli);

                close(client_fd);
                events[i].data.fd = -1;
                continue;
            }

            if (events[i].events & EPOLLHUP) //有来自client关闭
            { 
                fprintf(stderr, "EPOLLHUP close \n");

                ikv_free_client(cli->value);
                ikv_del_list_node(ikv_server.clients, cli);

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


void ikv_init_server()
{

    ldb_create_commands_table();
    
    server.Listen();
    ikv_sockfd = server.fd(); 

    int ret = ikv_set_sock_no_block( ikv_sockfd );
    if (ret == -1) {
        close(ikv_sockfd);
        return ;
    }

    struct epoll_event ev ;

    epfd = epoll_create( LDB_EVENT_MAX_COUNTS );

    ev.data.fd = ikv_sockfd; 
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, ikv_sockfd, &ev); 
    if (ret == -1) {
        close(ikv_sockfd);
        close(epfd);
        printf("epoll_ctl failed %s\n", strerror(errno));
        return ;
    }

    //if (server.daemonize_ ) ldb_daemon();

}


int main(int argc, char **argv)
{

    fprintf(stderr, "before init server\n");
    ikv_init_server(); 
    fprintf(stderr, "after init serverd\n");

    ikv_process_events();

#if 0
#endif
    return 0;
}            
