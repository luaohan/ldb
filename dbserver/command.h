// command.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_COMMAND_H_
#define _LDB_COMMAND_H_

namespace ldb {
namespace dbserver {

class Server;
class Client;

typedef void command_proc(Server *server, Client *c);

struct Command 
{
    const char *name;      // 命令名字
    command_proc *proc;     // 实现函数
    int argc;      // 参数个数，用于检查命令请求的格式是否正确，
                   // 如果这个值为负数 -N，那么参数的数量大于等于 N，
                   // 注意命令本身也是一个参数
    const char *sflags;   // 字符串表示的 FLAG
};

} /*namespace ldb*/
} /*namespace dbserver*/

#endif
