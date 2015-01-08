#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <errno.h>
#include <leveldb/db.h>
#include <leveldb/slice.h>

#include "string_type.h"
#include "server.h"
#include "command.h"
#include "../util/protocol.h"


void ldb_set_command(Server *server, Client *client)
{
    assert(server != NULL && client != NULL);

    leveldb::Slice key(client->key_, client->key_len_);
    
    int value_len = client->body_len_ - client->key_len_;
    leveldb::Slice val(client->val_, value_len);

    int ret = server->Insert(key, val);
    if (ret == -1) {
        //error
    }
    
    ret = FillPacket(client->replay_, MAX_PACKET_LEN, NULL, 0, NULL, 0, REPLAY_OK);
    int write_len = client->link_->WriteData(client->replay_, ret);
    if (write_len < ret) {
        if (errno != EAGAIN) {
            //error
            return;
        }
    }
 
    return ;
}

void ldb_get_command(Server *server, Client *client)
{
#if 0
    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR) + 1);
        return ;
    }

    leveldb::Slice s_key(client->argv_[1], strlen(client->argv_[1]));
    std::string s_val;

    int ret = server->Get(s_key, &s_val);
    if (ret == 0) {
        memcpy(client->replay_, s_val.c_str(), s_val.size() + 1);
    } else {
        memcpy(client->replay_, LDB_NO_THE_KEY, strlen(LDB_NO_THE_KEY) + 1);
    }
    
    return ;
#endif
}
#if 0
void ldb_update_command(Server *server, Client *client)
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
#endif

void ldb_del_command(Server *server, Client *client)
{
#if 0
    if (client->argc_ != client->cmd->argc) {
        memcpy(client->replay_, LDB_PARA_ERROR, strlen(LDB_PARA_ERROR) + 1);
        return ;
    }
  
    leveldb::Slice s_key(client->argv_[1], strlen(client->argv_[1]));
    std::string s_val;

    int ret = server->Delete(s_key);
    if (ret == 0) {
        memcpy(client->replay_, LDB_DEL_OK, strlen(LDB_DEL_OK) + 1);
    } else {
        memcpy(client->replay_, LDB_NO_THE_KEY, strlen(LDB_NO_THE_KEY) + 1);
    }
    
    return ;

#endif


#if 0 
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
#endif
}

#if 0
void ldb_lookall_command(Server *server, Client *client)
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

void ldb_clear_command(Server *server, Client *client)
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

void ldb_select_command(Server *server, Client *client)
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
