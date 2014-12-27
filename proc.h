// pro.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _PROC_H_
#define _PROC_H_

#include "server.h"
#include "client.h"

#define LDB_SPACE "                                          "
#define LDB_NO_THE_COMMAND "no have the command"

int ldb_tell_client( Client *client );

void process_client_info(Server &server, Client *client);

void ldb_process_events(Server &server);

#endif
