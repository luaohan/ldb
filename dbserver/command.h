// command.h (2014-12-23)
// WangPeng (1245268612@qq.com)

#ifndef _LDB_COMMAND_H_
#define _LDB_COMMAND_H_

class Server;
class Client;

typedef int CommandProc(Server *server, Client *client);

//ok: return 0
//error: return -1
int SetCommand(Server *server, Client *client);
int GetCommand(Server *server, Client *client);
int DelCommand(Server *server, Client *client);


#endif
