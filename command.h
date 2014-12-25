// ldb_command.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_COMMAND_H_
#define _LDB_COMMAND_H_

#include "server.h"
#include "client.h"

class Server;
class Client;

typedef void ldb_command_proc(Server *server, Client *c);

struct Command 
{
    char *name;      // 命令名字

    ldb_command_proc *proc;     // 实现函数

    int argc;      // 参数个数，用于检查命令请求的格式是否正确，
                   // 如果这个值为负数 -N，那么参数的数量大于等于 N，
                   // 注意命令本身也是一个参数

    char *sflags;   // 字符串表示的 FLAG
#if 0
    Command(char *i_name, ldb_command_proc *i_proc, int i_argc, char *i_sflags) {
        name = i_name;
        proc = i_proc;
        argc = i_argc;
        sflags = i_sflags;
    }
#endif
};



#endif
