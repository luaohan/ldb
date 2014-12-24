// ikv_string_type.h (2014-11-20)
// WangPeng (1245268612@qq.com)

#ifndef _IKV_STRING_TYPE_H_
#define _IKV_STRING_TYPE_H_

#include "ldb_client.h"
#include "ldb_server.h"

#define IKV_PARA_ERROR "para error"
#define IKV_KEY_EXIST "the key existed" 
#define IKV_ADD_OK "add key ok"
#define IKV_UPDATE_OK "update key ok"
#define IKV_UPDATE_ERROR "update key error"
#define IKV_NO_THE_KEY "no the key"
#define IKV_DEL_OK "del key ok"
#define IKV_DEL_ERROR "del key error"
#define IKV_CLEAR_OK "clear ok"
#define IKV_SELECT_OK "select ok"
#define IKV_SELECT_ERROR "select error"

void ikv_set_command(Server *server, Client *client);

void ikv_get_command(Server *server, Client *client);

void ikv_update_command(Server *server, Client *client);

void ikv_del_command(Server *server, Client *client);

void ikv_lookall_command(Server *server, Client *client);

void ikv_clear_command(Server *server, Client *client);

void ikv_select_command(Server *server, Client *client);

#endif
