#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "string_type.h"
#include "server.h"
#include "command.h"

namespace ldb {
namespace dbserver {

void set_command(Server *server, Client *client)
{
    assert(server != NULL && client != NULL);

    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }
    
    int ret; 
    int key_len = strlen(client->argv_[1]);
    char *key = (char *)malloc( key_len + 1);
    if (key == NULL) {
        return ;
    }
    
    //fprintf(stderr, "sizeof(key): %d\n", key_len + 1);
    memcpy(key, client->argv_[1], key_len);
    key[key_len] = '\0';

    int value_len = strlen(client->argv_[2]);
    char *value = (char *)malloc( value_len + 1);
    if (value== NULL) {
        return ;
    }

    //fprintf(stderr, "sizeof(value): %d\n", value_len + 1);
    memcpy(value, client->argv_[2], value_len);
    value[value_len] = '\0';

    server->Insert(std::string(key, key_len), std::string(value, value_len));

    memcpy(client->replay_, ADD_OK, strlen(ADD_OK));
    
    return ;
}

void get_command(Server *server, Client *client)
{
    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }

    std::string s_val;

    int ret = server->Get(client->argv_[1], &s_val);
    if (ret == 0) {
        memcpy(client->replay_, s_val.c_str(), s_val.size());
    } else {
        memcpy(client->replay_, NO_THE_KEY, strlen(NO_THE_KEY));
    }
    
    return ;
}

void del_command(Server *server, Client *client)
{
    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }
  
    leveldb::Slice s_key(client->argv_[1], strlen(client->argv_[1]));
    std::string s_val;

    int ret = server->Delete(client->argv_[1]);
    if (ret == 0) {
        memcpy(client->replay_, DEL_OK, strlen(DEL_OK));
    } else {
        memcpy(client->replay_, NO_THE_KEY, strlen(NO_THE_KEY));
    }
    
    return ;
}


} /*namespace ldb*/
} /*namespace dbserver*/

