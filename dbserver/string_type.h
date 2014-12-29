// string_type.h (2014-11-20)
// WangPeng (1245268612@qq.com)

#ifndef _STRING_TYPE_H_
#define _STRING_TYPE_H_

#include "client.h"
#include "server.h"

#define PARA_ERROR "para error"
#define KEY_EXIST "the key existed" 
#define ADD_OK "add key ok"
#define UPDATE_OK "update key ok"
#define UPDATE_ERROR "update key error"
#define NO_THE_KEY "no the key"
#define DEL_OK "del key ok"
#define DEL_ERROR "del key error"
#define CLEAR_OK "clear ok"
#define SELECT_OK "select ok"
#define SELECT_ERROR "select error"

void set_command(Server *server, Client *client);

void get_command(Server *server, Client *client);

//void update_command(Server *server, Client *client);

void del_command(Server *server, Client *client);

void lookall_command(Server *server, Client *client);

void clear_command(Server *server, Client *client);

void select_command(Server *server, Client *client);

#endif
