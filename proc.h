// pro.h (2014-12-25)
// WangPeng (1245268612@qq.com)

#ifndef _PROC_H_
#define _PROC_H_

#include "client.h"

#define LDB_SPACE "                                          "
#define LDB_NO_THE_COMMAND "no have the command"

int ldb_write(int fd, void *buf, int len);

int ldb_tell_client( Client *client );

void process_client_info(Client *client);


#endif
