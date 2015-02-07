// benchmark.h (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <string>

#define MAX_KEY_LEN 1024 * 60       //60 K
#define MAX_VAL_LEN 1024 * 1024 * 9 // 9 M

class Client;

class Benchmark {
    public:
        Benchmark(Client *client);
        ~Benchmark(){};

        void Run();

    private:
        void Test(int key_len, int val_len, int n);

    private:
        Client *client_;
        
};
#endif
