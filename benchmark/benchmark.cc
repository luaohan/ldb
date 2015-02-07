// benchmark.cc (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>

#include <dbclient/client.h>
#include <dbclient/status.h>

#include "benchmark.h"

Benchmark::Benchmark(Client *client):
    client_(client)
{
}

void Benchmark::Run()
{
    //      1K    1K
    //Test(1024, 1024, 100);
    //      1K    40K 
    Test(1024, 1024 * 40, 100);
    //      1K    400K 
    //Test(1024, 1024 * 400, 100);
}

void Benchmark::Test(int key_len, int val_len, int n)
{
    assert(key_len <= MAX_KEY_LEN && key_len >= 5);
    assert(val_len <= MAX_VAL_LEN && val_len >= 5);

    long long set_time = 0; //ms
    long long get_time = 0; //ms
    long long del_time = 0; //ms

    struct timeval t1;
    struct timeval t2;

    char key[key_len];
    char val[val_len];

    memset(key, '1', key_len);
    memset(val, '1', val_len);

    for (int i = 0; i < n; i++) 
    {
        printf("%d\n", i);

        sprintf(key + key_len - 5, "%05d", i);
        sprintf(val + val_len - 5, "%05d", i);

        std::string true_key(key, key_len);
        std::string true_val(val, val_len);
 
        //printf("keylen:%d, vallen:%d\n", true_key.size(), true_val.size());
        //printf("key:|%s|, val:|%s|\n", true_key.c_str(), true_val.c_str());
        
        Status s;
        
        gettimeofday(&t1, NULL);
        s = client_->Set(true_key, true_val);
        gettimeofday(&t2, NULL);
        assert(s.IsOk());
        set_time += ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
       
        std::string get_val;
        gettimeofday(&t1, NULL);
        s = client_->Get(true_key, &get_val);
        gettimeofday(&t2, NULL);
        assert(s.IsOk());
        get_time += ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
        
        gettimeofday(&t1, NULL);
        s = client_->Del(true_key);
        gettimeofday(&t2, NULL);
        assert(s.IsOk());
        del_time += ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec));
    }

    printf("set_time:\n");
    printf("key_len:%d K, val_len:%d K\n", key_len/1024, val_len/1024);
    printf("all_time:%d ms\n", set_time);
    //printf("n == %d, %d data/s \n", n, n / (set_time / 1000000 ));
    printf("\n");
    
    printf("get_time:\n");
    printf("key_len:%d K, val_len:%d K\n", key_len/1024, val_len/1024);
    printf("all_time:%d ms\n", get_time);
    //printf("n == %d, %d data/s \n", n, n / (get_time / 1000000 ));
    printf("\n");
    
    printf("del_time:\n");
    printf("key_len:%d K, val_len:%d K\n", key_len/1024, val_len/1024);
    printf("all_time:%d ms\n", del_time);
    //printf("n == %d, %d data/s \n", n, n / (del_time / 1000000 ));
}
