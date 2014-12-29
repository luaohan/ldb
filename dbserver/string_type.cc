#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <leveldb/db.h>
#include <iostream>

#include "string_type.h"
#include "server.h"
#include "command.h"


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

    leveldb::Slice s_key(key, key_len);
    leveldb::Slice s_val(value, value_len);

    server->Insert(s_key, s_val);

    memcpy(client->replay_, ADD_OK, strlen(ADD_OK));
    
    return ;
}

void get_command(Server *server, Client *client)
{
    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }

    leveldb::Slice s_key(client->argv_[1], strlen(client->argv_[1]));
    std::string s_val;

    int ret = server->Get(s_key, &s_val);
    if (ret == 0) {
        memcpy(client->replay_, s_val.c_str(), s_val.size());
    } else {
        memcpy(client->replay_, NO_THE_KEY, strlen(NO_THE_KEY));
    }
    
    return ;
}

#if 0
void update_command(Server *server, Client *client)
{
    int ret; 
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }
  
    hash_table_node_t *old_node = 
        find_dict(server->db[client->dictid].dict, client->argv[1]);
    if (old_node == NULL) {
        memcpy(client->buf, NO_THE_KEY, strlen(NO_THE_KEY));
        return ; 
    }

    int old_value_len = strlen(old_node->value);
    
    free(old_node->value);
    TOTAL_MALLOC -= (old_value_len + 1);
    
    int new_value_len = strlen(client->argv[2]);
    char *new_value = malloc( new_value_len + 1);
    if (new_value == NULL) {
        return ;
    }
    TOTAL_MALLOC += (new_value_len + 1);

    fprintf(stderr, "sizeof(new_value): %d\n", new_value_len + 1);

    memcpy(new_value, client->argv[2], new_value_len);
    new_value[new_value_len] = '\0';

    ret = 
        replace_dict((server->db[client->dictid]).dict, old_node->key, new_value);
    if (ret != 0) {
        memcpy(client->buf, UPDATE_ERROR, strlen(UPDATE_ERROR));
        return ;
    }

    memcpy(client->buf, UPDATE_OK, strlen(UPDATE_OK));
    fprintf(stderr, "update string later:%d\n", TOTAL_MALLOC);
}
#endif

void del_command(Server *server, Client *client)
{
    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }
  
    leveldb::Slice s_key(client->argv_[1], strlen(client->argv_[1]));
    std::string s_val;

    int ret = server->Delete(s_key);
    if (ret == 0) {
        memcpy(client->replay_, DEL_OK, strlen(DEL_OK));
    } else {
        memcpy(client->replay_, NO_THE_KEY, strlen(NO_THE_KEY));
    }
    
    return ;

#if 0 
    hash_table_node_t *old_node = 
        find_dict(server->db[client->dictid].dict, client->argv[1]);
    if (old_node == NULL) {
        memcpy(client->buf, NO_THE_KEY, strlen(NO_THE_KEY));
        return ; 
    }

    int old_key_len = strlen(old_node->key);
    int old_value_len = strlen(old_node->value);
    
    ret = delete_dict((server->db[client->dictid]).dict, old_node->key);
    if (ret != 0) {
        memcpy(client->buf, DEL_ERROR, strlen(DEL_ERROR));
        return ;
    }
    
    free(old_node->key);
    free(old_node->value);
    TOTAL_MALLOC -= (old_key_len + 1);
    TOTAL_MALLOC -= (old_value_len + 1);
    
    memcpy(client->buf, DEL_OK, strlen(DEL_OK));

    fprintf(stderr, "del string later:%d\n", TOTAL_MALLOC);
#endif
}

#if 0
void lookall_command(Server *server, Client *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }
    
    char *k_buf[10];
    char *v_buf[10];
    char buf[1024];
    int times, i;    

    print_all_keys((server->db[client->dictid]).dict, k_buf, v_buf, 1, &times);
    
    sprintf(buf, "   total: %d keys\n", times);
    strcat(client->buf, buf);

    for (i = 0; i < times; i++) {
        char str_buf[1024] = {0};
        sprintf(str_buf, "\tkey(%s)--->value(%s)\n", k_buf[i], v_buf[i]);
        strcat(client->buf, str_buf);
    }

}

void clear_command(Server *server, Client *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }
    
    char *k_buf[10];
    char *v_buf[10];
    char buf[1024];
    int times, i;    
    int ret;

    print_all_keys((server->db[client->dictid]).dict, k_buf, v_buf, 1, &times);
    
    sprintf(buf, "   total: %d keys\n", times);
    strcat(client->buf, buf);

    for (i = 0; i < times; i++) {
        
        int old_key_len = strlen(k_buf[i]);
        int old_value_len = strlen(v_buf[i]);

        ret = delete_dict((server->db[client->dictid]).dict, k_buf[i]);
        if (ret != 0) {
            memcpy(client->buf, DEL_ERROR, strlen(DEL_ERROR));
            return ;
        }

        free(k_buf[i]);
        free(v_buf[i]);
        TOTAL_MALLOC -= (old_key_len + 1);
        TOTAL_MALLOC -= (old_value_len + 1);
    }

    strcat(client->buf, CLEAR_OK);

    fprintf(stderr, "clear later:%d\n", TOTAL_MALLOC);
}

void select_command(Server *server, Client *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, PARA_ERROR, strlen(PARA_ERROR));
        return ;
    }
  
    int num = atoi(client->argv[1]);
    if (num >= server->dbnum || num < 0) {
        memcpy(client->buf, SELECT_ERROR, strlen(SELECT_ERROR));
        return ;
    } 
    
    client->dictid = num;

    memcpy(client->buf, SELECT_OK, strlen(SELECT_OK));

}
#endif
