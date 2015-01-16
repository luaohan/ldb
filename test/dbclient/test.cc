// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <string>

#include <dbclient/ldbc.h>

#define TWO_M 1024 * 1024 * 20 // 20 M

int main()
{
#if 0   
    Client cli(false); //非分布式

    if (cli.Connect("127.0.0.1", 8899) == -1) {
        fprintf(stderr, "connect errno: %s\n", strerror(errno));
        return -1;
    }
#endif

    Client cli(true); //非分布式
    std::string file_name = "config.json";
    int ret = cli.Init(file_name);
    if (ret == -1) {
        fprintf(stderr, "connect errno: %s\n", strerror(errno));
        return -1;
    } else if (ret == -2) {
        fprintf(stderr, "configfile error\n");
        return -1;
    }
    
    char key[1024];
    memset(key, '1', 1023);
    key[1023] = '\0';
    
    //char val[TWO_M];
    char *val = (char *)malloc(TWO_M);
    if (val == NULL) {
        printf("malloc error\n");
        return -1;
    }
    memset(val, '2', TWO_M - 1);
    val[TWO_M - 1] = '\0';

    std::string true_key(key, 1024);
    std::string true_val(val, TWO_M);

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

    return 0;
}
