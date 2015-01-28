// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <string>

#include <dbclient/client.h>

#define val_len 1024 * 1024 * 2 // 2 M

int main()
{
    Client cli(false, "config.json"); //非分布式

    if (cli.Connect("192.168.0.1", 8881) == -1) {
        fprintf(stderr, "connect errno: %s\n", strerror(errno));
        return -1;
    }
    return 0;




    char key[1024];
    memset(key, '1', 1023);
    key[1023] = '\0';
    
    char *val = (char *)malloc(val_len);
    if (val == NULL) {
        printf("malloc error\n");
        return -1;
    }
    memset(val, '2', val_len - 1);
    val[val_len - 1] = '\0';

    std::string true_key(key, 1024);
    std::string true_val(val, val_len);

    int t1 = time(NULL);
    //for (int i = 0; i < 100000; i++) {
    Status s = cli.Set(true_key, true_val);
    if (s.IsOk()) {
        printf("Set ok\n");
    } else {
        printf("Set error, ret : %s\n", s.ToString().c_str());
    }
    //}

    std::string get;
    s = cli.Get(true_key, &get);
    if (s.IsKeyNotExist()) {
        printf("key not exit\n");
    } else {
        printf("value_len: %d\n", get.size());
    }

    cli.Del(true_key);
    s = cli.Get(true_key, &get);
    if (s.IsKeyNotExist()) {
        printf("key not exit\n");
    } else {
        printf("value: %s\n", get.c_str());
    }
    int t = time(NULL) - t1;
    printf("100000 datas, time is : %d seconds, %d/s\n", t, 100000/t);

#if 0   
    Client cli(true); //分布式
    std::string file_name = "config.json";
    int ret = cli.Init(file_name);
    if (ret == -1) {
        fprintf(stderr, "connect errno: %s\n", strerror(errno));
        return -1;
    } else if (ret == -2) {
        fprintf(stderr, "configfile error\n");
        return -1;
    }

    int max_i = 100;

    int key_length = 5;
    char key[key_length];
    
    int val_length = 1024 * 512; //512 K
    char val[val_length];
    memset(val, '1', val_length);
    val[val_length] = '\0';
    std::string true_val(val, val_length);
    std::string getval;
    Status s;

    for(int i = 0; i < max_i; i++) {
        sprintf(key, "%d", i);
        key[key_length] = '\0';
        std::string true_key(key, key_length);
        cli.Set(true_key, true_val);
        cli.Get(true_key, &getval);
        if (strcmp(getval.c_str(), true_val.c_str()) != 0) {
            printf("getval error\n");
            return -1;
        }
        
        cli.Del(true_key);
        s = cli.Get(true_key, &getval); 
        assert(s.IsKeyNotExist());
    }
#endif

    return 0;
}
