// 1.cc (2014-12-13)
// WangPeng (1245268612@qq.com)

#include <assert.h>  
#include <string.h>  
#include <sys/time.h>
#include <leveldb/db.h>  
#include <iostream>  

void TestSet(leveldb::DB* db, int key_len, int val_len, int n)
{
    long long set_time = 0; //us
    long long get_time = 0; //us
    long long del_time = 0; //us
    long long time_pos = 0; //us

    struct timeval t1;
    struct timeval t2;

    char key[key_len];
    char val[val_len];

    memset(key, '1', key_len);
    memset(val, '1', val_len);
    
    for (int i = 0; i < n; i++) 
    {
        sprintf(key + key_len - 5, "%05d", i);
        sprintf(val + val_len - 5, "%05d", i);

        std::string true_key(key, key_len);
        std::string true_val(val, val_len);
 
        leveldb::Status s;
        
        gettimeofday(&t1, NULL);
        s = db->Put(leveldb::WriteOptions(), true_key, true_val);
        gettimeofday(&t2, NULL);
        assert(s.ok());
        time_pos = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);

        //if (time_pos < 1000000) 
            set_time += time_pos;
        //if (time_pos > 1000) 
            printf("%d, set_time: %d\t", i, time_pos);
    }
    
    printf("set_time:\n");
    printf("key_len:%d K, val_len:%d K\n", key_len/1024, val_len/1024);
    printf("all_time:%d ns\n", set_time);
    printf("n == %d, %d data/s \n", n, n / (set_time / 1000000 ));
    printf("\n");
}

int main(int argc, char *argv[])
{  
    leveldb::DB* db;  
    leveldb::Options options;  
    options.create_if_missing = true;  

    leveldb::Status status = leveldb::DB::Open(options, "/tmp/test_level_db", &db);  
    assert( status.ok() );  

    TestSet(db, 1024, 1024, 10000);

    delete db;  
    printf("ok\n");
    return 0;  
}  
