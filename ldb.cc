// ldb.cc (2014-12-23)
// WangPeng (1245268612@qq.com)

#include <iostream>

#include "ldb_server.h"
#include "ldb_command.h"

#define LDB_EVENT_MAX_COUNTS 1024
#define LDB_SPACE "                                          "
#define LDB_NO_THE_COMMAND "no have the command"

Server server(8899);
int ikv_sockfd = 0;          
int epfd = 0;
struct epoll_event ev, events[IKV_EVENT_MAX_COUNTS];

Command ldb_commands_table[] = {
    {"set", ikv_set_command, 3, "w"},
    {"get", ikv_get_command, 2, "r"},
    {"update", ikv_update_command, 3, "w"},
    {"del", ikv_del_command, 2, "w"},
    {"lookall", ikv_lookall_command, 1, "r"},
    {"clear", ikv_clear_command, 1, "w"},
    {"select", ikv_select_command, 2, "w"}
};  

int main(int argc, char *argv[])
{

}
