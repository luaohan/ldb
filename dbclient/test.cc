// test.cc (2014-12-25)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

#include "ldbc.h"
#include "../util/protocol.h"

int main()
{
    Client cli;

    if (cli.socket_.Connect("127.0.0.1", 8899) == -1) {
        fprintf(stderr, "connect errno: %s\n", strerror(errno));
        return -1;
    }


    char key[10];
    memset(key, '1', 9);
    key[9] = '\0';
    
    char val[10];
    memset(val, '2', 9);
    val[9] = '\0';

    std::string true_key(key, 10);
    std::string true_val(val, 10);

    int t1 = time(NULL);
#if 0
    for (int i = 0; i < 100000; i++) {
        cli.Set(true_key, true_val);
    }
#endif
    
    std::string get;
    cli.Get(true_key, &get);
    printf("value: %s\n", get.c_str());

    cli.Del(true_key);
    int ret = cli.Get(true_key, &get);
    if (ret == 4) {
        printf("key not exit\n");
    } else {
        printf("value: %s\n", get.c_str());
    }

    int t = time(NULL) - t1;
    printf("100000 datas, time is : %d seconds, %d/s\n", t, 100000/t);

    return 0;
}
