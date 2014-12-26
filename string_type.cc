#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <leveldb/db.h>
#include <iostream>

#include "string_type.h"
#include "server.h"
#include "command.h"


void ldb_set_command(Server *server, Client *client)
{
    assert(server != NULL && client != NULL);

    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR));
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
    ret = ldb_add_dict((server->db[client->dictid]).dict, key, value);
    if (ret == -1) {
        memcpy(client->buf, LDB_KEY_EXIST, strlen(LDB_KEY_EXIST));
        return ;
    }
#endif

    memcpy(client->replay_, LDB_ADD_OK, strlen(LDB_ADD_OK));
    
    return ;
}

#if 0
void ldb_get_command(ldb_server_t *server, ldb_client_t *client)
{
    char *ret = NULL; 
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR));
        return ;
    }

    ret = ldb_fetch_value((server->db[client->dictid]).dict, client->argv[1]);
    if (ret == NULL) {
        memcpy(client->buf, LDB_NO_THE_KEY, strlen(LDB_NO_THE_KEY));
        return ;
    }
    
    memcpy(client->buf, ret, strlen(ret));
}

void ldb_update_command(ldb_server_t *server, ldb_client_t *client)
{
    int ret; 
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR));
        return ;
    }
  
    ldb_hash_table_node_t *old_node = 
        ldb_find_dict(server->db[client->dictid].dict, client->argv[1]);
    if (old_node == NULL) {
        memcpy(client->buf, LDB_NO_THE_KEY, strlen(LDB_NO_THE_KEY));
        return ; 
    }

    int old_value_len = strlen(old_node->value);
    
    free(old_node->value);
    ldb_TOTAL_MALLOC -= (old_value_len + 1);
    
    int new_value_len = strlen(client->argv[2]);
    char *new_value = malloc( new_value_len + 1);
    if (new_value == NULL) {
        return ;
    }
    ldb_TOTAL_MALLOC += (new_value_len + 1);

    fprintf(stderr, "sizeof(new_value): %d\n", new_value_len + 1);

    memcpy(new_value, client->argv[2], new_value_len);
    new_value[new_value_len] = '\0';

    ret = 
        ldb_replace_dict((server->db[client->dictid]).dict, old_node->key, new_value);
    if (ret != 0) {
        memcpy(client->buf, LDB_UPDATE_ERROR, strlen(LDB_UPDATE_ERROR));
        return ;
    }

    memcpy(client->buf, LDB_UPDATE_OK, strlen(LDB_UPDATE_OK));
    fprintf(stderr, "update string later:%d\n", ldb_TOTAL_MALLOC);
}

void ldb_del_command(ldb_server_t *server, ldb_client_t *client)
{
    int ret; 
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR));
        return ;
    }
  
    ldb_hash_table_node_t *old_node = 
        ldb_find_dict(server->db[client->dictid].dict, client->argv[1]);
    if (old_node == NULL) {
        memcpy(client->buf, LDB_NO_THE_KEY, strlen(LDB_NO_THE_KEY));
        return ; 
    }

    int old_key_len = strlen(old_node->key);
    int old_value_len = strlen(old_node->value);
    
    ret = ldb_delete_dict((server->db[client->dictid]).dict, old_node->key);
    if (ret != 0) {
        memcpy(client->buf, LDB_DEL_ERROR, strlen(LDB_DEL_ERROR));
        return ;
    }
    
    free(old_node->key);
    free(old_node->value);
    ldb_TOTAL_MALLOC -= (old_key_len + 1);
    ldb_TOTAL_MALLOC -= (old_value_len + 1);
    
    memcpy(client->buf, LDB_DEL_OK, strlen(LDB_DEL_OK));

    fprintf(stderr, "del string later:%d\n", ldb_TOTAL_MALLOC);
}

void ldb_lookall_command(ldb_server_t *server, ldb_client_t *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR));
        return ;
    }
    
    char *k_buf[10];
    char *v_buf[10];
    char buf[1024];
    int times, i;    

    ldb_print_all_keys((server->db[client->dictid]).dict, k_buf, v_buf, 1, &times);
    
    sprintf(buf, "   total: %d keys\n", times);
    strcat(client->buf, buf);

    for (i = 0; i < times; i++) {
        char str_buf[1024] = {0};
        sprintf(str_buf, "\tkey(%s)--->value(%s)\n", k_buf[i], v_buf[i]);
        strcat(client->buf, str_buf);
    }

}

void ldb_clear_command(ldb_server_t *server, ldb_client_t *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR));
        return ;
    }
    
    char *k_buf[10];
    char *v_buf[10];
    char buf[1024];
    int times, i;    
    int ret;

    ldb_print_all_keys((server->db[client->dictid]).dict, k_buf, v_buf, 1, &times);
    
    sprintf(buf, "   total: %d keys\n", times);
    strcat(client->buf, buf);

    for (i = 0; i < times; i++) {
        
        int old_key_len = strlen(k_buf[i]);
        int old_value_len = strlen(v_buf[i]);

        ret = ldb_delete_dict((server->db[client->dictid]).dict, k_buf[i]);
        if (ret != 0) {
            memcpy(client->buf, LDB_DEL_ERROR, strlen(LDB_DEL_ERROR));
            return ;
        }

        free(k_buf[i]);
        free(v_buf[i]);
        ldb_TOTAL_MALLOC -= (old_key_len + 1);
        ldb_TOTAL_MALLOC -= (old_value_len + 1);
    }

    strcat(client->buf, LDB_CLEAR_OK);

    fprintf(stderr, "clear later:%d\n", ldb_TOTAL_MALLOC);
}

void ldb_select_command(ldb_server_t *server, ldb_client_t *client)
{
    if (client->argc != client->cmd->argc) {
        memcpy(client->buf, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR));
        return ;
    }
  
    int num = atoi(client->argv[1]);
    if (num >= server->dbnum || num < 0) {
        memcpy(client->buf, LDB_SELECT_ERROR, strlen(LDB_SELECT_ERROR));
        return ;
    } 
    
    client->dictid = num;

    memcpy(client->buf, LDB_SELECT_OK, strlen(LDB_SELECT_OK));

}
#endif
