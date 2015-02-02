// main.cc (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <iostream>
#include <dbclient/client.h>
#include "test.h"

using namespace std;

int main(int argc, char *argv[])
{
    std::string slave_conf("slave_conf.json");
    Client cli(false, slave_conf); //非分布式
    if (cli.Connect("127.0.0.1", 8899) == false) {
        cout << "connect error: " << strerror(errno) << ", errno: " << errno << endl;
        return -1;
    }
#if 0    
    std::string a_key("a");
    std::string b_key("b");
    std::string c_key("c");
    std::string d_key("d");
    std::string e_key("e");
    std::string f_key("f");
    std::string g_key("g");
    std::string h_key("h");
    
    std::string a_val("aaaaaaaaaa");
    std::string b_val("bbbbbbbbbb");
    std::string c_val("cccccccccc");
    std::string d_val("dddddddddd");
    std::string e_val("eeeeeeeeee");
    std::string f_val("ffffffffff");
    std::string g_val("gggggggggg");
    std::string h_val("hhhhhhhhhh");
    
    Status s;
    s = cli.Set(a_key, a_val);
    assert(s.IsOk());
    cli.Set(b_key, b_val);
    assert(s.IsOk());
    cli.Set(c_key, c_val);
    assert(s.IsOk());
    cli.Set(d_key, d_val);
    assert(s.IsOk());
    cli.Set(e_key, e_val);
    assert(s.IsOk());
    cli.Set(f_key, f_val);
    assert(s.IsOk());
    cli.Set(g_key, g_val);
    assert(s.IsOk());
    cli.Set(h_key, h_val);
    assert(s.IsOk());
    
    printf("set over \n");
  
    sleep(5);   //这时关掉 master 
                //下面就是从 slave 中获取

    std::string get;
    cli.Get(a_key, &get);
    printf("val: |%s|\n", get.c_str());
    cli.Get(b_key, &get);
    printf("val: |%s|\n", get.c_str());
    cli.Get(c_key, &get);
    printf("val: |%s|\n", get.c_str());
    cli.Get(d_key, &get);
    printf("val: |%s|\n", get.c_str());
    cli.Get(e_key, &get);
    printf("val: |%s|\n", get.c_str());
    cli.Get(f_key, &get);
    printf("val: |%s|\n", get.c_str());
    cli.Get(g_key, &get);
    printf("val: |%s|\n", get.c_str());
    cli.Get(h_key, &get);
    printf("val: |%s|\n", get.c_str());
#endif
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
    
    printf("init ok\n");
#endif

    Test t(&cli);
    t.Run();

    return 0;
}
