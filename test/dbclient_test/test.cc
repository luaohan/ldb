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

#define key_length  5
#define val_length  5

int main()
{
    std::string file_name = "config.json";
    Client cli(file_name); 
    int ret = cli.Init();
    if (ret != 0) {
        fprintf(stderr, "ret: %d, connect errno: %s\n", ret, strerror(errno));
        return -1;
    }

    int max_i = 100000;

    char key[key_length];
    memset(key, 'k', key_length);
    key[key_length - 1] = '\0';

    char val[val_length];
    memset(val, 'v', val_length);
    val[val_length -1] = '\0';

    std::string true_key(key, key_length);
    std::string true_val(val, val_length);

    Status s;
    s = cli.Set(true_key, true_val);
    assert(s.IsOk());

    std::string geted_val;
    for(int i = 0; i < max_i; i++) {
        s = cli.Get(true_key, &geted_val);
        assert(s.IsOk());
        if (geted_val != true_val) {
            printf("|%s|, %d\n", true_val.c_str(), true_val.size());
            printf(">>|%s|, %d\n", geted_val.c_str(), geted_val.size());
            return -1;
        }
    }

    return 0;
}
