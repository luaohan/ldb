// ldb_string_type.h (2014-11-20)
// WangPeng (1245268612@qq.com)

#ifndef _ldb_STRING_TYPE_H_
#define _ldb_STRING_TYPE_H_

#include "client.h"
#include "server.h"

#define LDB_PARA_ERROR "para error"
#define LDB_KEY_EXIST "the key existed" 
#define LDB_ADD_OK "add key ok"
#define LDB_UPDATE_OK "update key ok"
#define LDB_UPDATE_ERROR "update key error"
#define LDB_NO_THE_KEY "no the key"
#define LDB_DEL_OK "del key ok"
#define LDB_DEL_ERROR "del key error"
#define LDB_CLEAR_OK "clear ok"
#define LDB_SELECT_OK "select ok"
#define LDB_SELECT_ERROR "select error"

void ldb_set_command(Server *server, Client *client);

void ldb_get_command(Server *server, Client *client);

void ldb_update_command(Server *server, Client *client);

void ldb_del_command(Server *server, Client *client);

void ldb_lookall_command(Server *server, Client *client);

void ldb_clear_command(Server *server, Client *client);

void ldb_select_command(Server *server, Client *client);

#endif
