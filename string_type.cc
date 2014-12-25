#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <leveldb/db.h>
#include <iostream>

#include "string_type.h"
#include "server.h"
#include "command.h"


void ikv_set_command(Server *server, Client *client)
{
    assert(server != NULL && client != NULL);

    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, IKV_PARA_ERROR, strlen(IKV_PARA_ERROR));
        return ;
    }
    
    int ret; 
    int key_len = strlen(client->argv_[1]);
    char *key = (char *)malloc( key_len + 1);
    if (key == NULL) {
        return ;
    }
    
    fprintf(stderr, "sizeof(key): %d\n", key_len + 1);
    memcpy(key, client->argv_[1], key_len);
    key[key_len] = '\0';

    int value_len = strlen(client->argv_[2]);
    char *value = (char *)malloc( value_len + 1);
    if (value== NULL) {
        return ;
    }

    fprintf(stderr, "sizeof(value): %d\n", value_len + 1);
    memcpy(value, client->argv_[2], value_len);
    value[value_len] = '\0';

    leveldb::Slice s_key(key, key_len);
    leveldb::Slice s_val(value, value_len);

    server->Insert(s_key, s_val);

#if 0
    ret = ikv_add_dict((server->db[client->dictid]).dict, key, value);
    if (ret == -1) {
        memcpy(client->buf, IKV_KEY_EXIST, strlen(IKV_KEY_EXIST));
        return ;
    }
#endif

    memcpy(client->replay_, IKV_ADD_OK, strlen(IKV_ADD_OK));
    
    return ;
}

#if 0
void ikv_get_command(ikv_server_t *server, ikv_client_t *client)
{
    char *ret = NULL; 
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, IKV_PARA_ERROR, strlen(IKV_PARA_ERROR));
        return ;
    }

    ret = ikv_fetch_value((server->db[client->dictid]).dict, client->argv[1]);
    if (ret == NULL) {
        memcpy(client->buf, IKV_NO_THE_KEY, strlen(IKV_NO_THE_KEY));
        return ;
    }
    
    memcpy(client->buf, ret, strlen(ret));
}

void ikv_update_command(ikv_server_t *server, ikv_client_t *client)
{
    int ret; 
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, IKV_PARA_ERROR, strlen(IKV_PARA_ERROR));
        return ;
    }
  
    ikv_hash_table_node_t *old_node = 
        ikv_find_dict(server->db[client->dictid].dict, client->argv[1]);
    if (old_node == NULL) {
        memcpy(client->buf, IKV_NO_THE_KEY, strlen(IKV_NO_THE_KEY));
        return ; 
    }

    int old_value_len = strlen(old_node->value);
    
    free(old_node->value);
    IKV_TOTAL_MALLOC -= (old_value_len + 1);
    
    int new_value_len = strlen(client->argv[2]);
    char *new_value = malloc( new_value_len + 1);
    if (new_value == NULL) {
        return ;
    }
    IKV_TOTAL_MALLOC += (new_value_len + 1);

    fprintf(stderr, "sizeof(new_value): %d\n", new_value_len + 1);

    memcpy(new_value, client->argv[2], new_value_len);
    new_value[new_value_len] = '\0';

    ret = 
        ikv_replace_dict((server->db[client->dictid]).dict, old_node->key, new_value);
    if (ret != 0) {
        memcpy(client->buf, IKV_UPDATE_ERROR, strlen(IKV_UPDATE_ERROR));
        return ;
    }

    memcpy(client->buf, IKV_UPDATE_OK, strlen(IKV_UPDATE_OK));
    fprintf(stderr, "update string later:%d\n", IKV_TOTAL_MALLOC);
}

void ikv_del_command(ikv_server_t *server, ikv_client_t *client)
{
    int ret; 
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, IKV_PARA_ERROR, strlen(IKV_PARA_ERROR));
        return ;
    }
  
    ikv_hash_table_node_t *old_node = 
        ikv_find_dict(server->db[client->dictid].dict, client->argv[1]);
    if (old_node == NULL) {
        memcpy(client->buf, IKV_NO_THE_KEY, strlen(IKV_NO_THE_KEY));
        return ; 
    }

    int old_key_len = strlen(old_node->key);
    int old_value_len = strlen(old_node->value);
    
    ret = ikv_delete_dict((server->db[client->dictid]).dict, old_node->key);
    if (ret != 0) {
        memcpy(client->buf, IKV_DEL_ERROR, strlen(IKV_DEL_ERROR));
        return ;
    }
    
    free(old_node->key);
    free(old_node->value);
    IKV_TOTAL_MALLOC -= (old_key_len + 1);
    IKV_TOTAL_MALLOC -= (old_value_len + 1);
    
    memcpy(client->buf, IKV_DEL_OK, strlen(IKV_DEL_OK));

    fprintf(stderr, "del string later:%d\n", IKV_TOTAL_MALLOC);
}

void ikv_lookall_command(ikv_server_t *server, ikv_client_t *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, IKV_PARA_ERROR, strlen(IKV_PARA_ERROR));
        return ;
    }
    
    char *k_buf[10];
    char *v_buf[10];
    char buf[1024];
    int times, i;    

    ikv_print_all_keys((server->db[client->dictid]).dict, k_buf, v_buf, 1, &times);
    
    sprintf(buf, "   total: %d keys\n", times);
    strcat(client->buf, buf);

    for (i = 0; i < times; i++) {
        char str_buf[1024] = {0};
        sprintf(str_buf, "\tkey(%s)--->value(%s)\n", k_buf[i], v_buf[i]);
        strcat(client->buf, str_buf);
    }

}

void ikv_clear_command(ikv_server_t *server, ikv_client_t *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, IKV_PARA_ERROR, strlen(IKV_PARA_ERROR));
        return ;
    }
    
    char *k_buf[10];
    char *v_buf[10];
    char buf[1024];
    int times, i;    
    int ret;

    ikv_print_all_keys((server->db[client->dictid]).dict, k_buf, v_buf, 1, &times);
    
    sprintf(buf, "   total: %d keys\n", times);
    strcat(client->buf, buf);

    for (i = 0; i < times; i++) {
        
        int old_key_len = strlen(k_buf[i]);
        int old_value_len = strlen(v_buf[i]);

        ret = ikv_delete_dict((server->db[client->dictid]).dict, k_buf[i]);
        if (ret != 0) {
            memcpy(client->buf, IKV_DEL_ERROR, strlen(IKV_DEL_ERROR));
            return ;
        }

        free(k_buf[i]);
        free(v_buf[i]);
        IKV_TOTAL_MALLOC -= (old_key_len + 1);
        IKV_TOTAL_MALLOC -= (old_value_len + 1);
    }

    strcat(client->buf, IKV_CLEAR_OK);

    fprintf(stderr, "clear later:%d\n", IKV_TOTAL_MALLOC);
}

void ikv_select_command(ikv_server_t *server, ikv_client_t *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, IKV_PARA_ERROR, strlen(IKV_PARA_ERROR));
        return ;
    }
  
    int num = atoi(client->argv[1]);
    if (num >= server->dbnum || num < 0) {
        memcpy(client->buf, IKV_SELECT_ERROR, strlen(IKV_SELECT_ERROR));
        return ;
    } 
    
    client->dictid = num;

    memcpy(client->buf, IKV_SELECT_OK, strlen(IKV_SELECT_OK));

}
#endif
