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

    int max_i = 100;

    char key[key_length];
    memset(key, 'k', key_length);
    key[key_length] = '\0';

    char val[val_length];
    memset(val, 'v', val_length);
    val[val_length] = '\0';

    std::string true_key(key, key_length);
    std::string true_val(val, val_length);

    s = cli.Set(true_key, true_val);
    assert(s.IsOk());

    std::string geted_val;
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

    return 0;
}
