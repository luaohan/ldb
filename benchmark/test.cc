// boundary.cc (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <dbclient/client.h>
#include <dbclient/status.h>
#include "test.h"

using namespace std;

#define ASSERT_TRUE(x)  if (!x) { assert(0); return false; }
#define ASSERT_FALSE(x)  if (x) { assert(0); return false; }

Test::Test(Client *client) 
    : client_(client), mod_(kText)
{
    for (int i = 0; i < 256; i++) {
        bin_[i]= i;
    }
}

bool Test::Basic(std::string &key, std::string &value, bool only_set)
{
    //cout << "Enter Basic test" << endl;

    mod_ = kText;

    bool ret = Exec(key, value, only_set);

    //cout << "Leave Basic test" << endl;

    return ret;
}

bool Test::Binary(int key_len, int value_len, bool only_set)
{
    //cout << "Enter Binary, test" << endl;

    mod_ = kBinary;

    std::string key;
    std::string value;

    for (int i = 0; i < key_len; i++) {
        key.append((char *)&bin_[0], sizeof(bin_));
    }

    //2M value
    for (int i = 0; i < value_len; i++) {
        value.append((char *)&bin_[0], sizeof(bin_));
    }

    bool ret = Exec(key, value, only_set);

    //cout << "Leave Binary, test" << endl;

    return ret;
}

bool Test::Batch(int count, int key_len, int value_len)
{
    //cout << "Enter Batch, test" << endl;

    mod_ = kBinary;

    for (int i = 0; i < count; i++) {
        bool ret = Binary(key_len, value_len);
        if (!ret) {
            return false;
        }
    }

    //cout << "Leave Batch, test" << endl;

    return true;
}

#define KEY(k) (mod_ == kText ? k : Hex(k))

bool Test::Exec(std::string &key, std::string &value, bool only_set)
{
    //cout << "test mode: " 
    //    << mod_ == kText ? "text" : "binary"  << endl;
    //cout << "key size: " << key.size() 
    //    << ", value size: " << value.size() << endl;

    
    Status status;
    status = client_->Set(key, value);
    if (!status.IsOk()) {
        exit(0);
        cout << "set key: " << KEY(key) << " failed" << endl;
        return false;
    }
    
    if (only_set) {
        return true;
    }
    std::string response;
    status = client_->Get(key, &response);
    if (!status.IsOk()) {
        exit(0);
        cout << "get key: " << KEY(key) << " failed" << endl;
        return false;
    }
    if (response != value) {
        exit(0);
        cout << "get key: " << KEY(key) 
            << " failed, invalid result: "
            << response << endl;
        return false;
    }
    status = client_->Del(key);
    if (!status.IsOk()) {
        exit(0);
        cout << "del key: " << KEY(key) << " failed" << endl;
        return false;
    }
    status = client_->Get(key, &response);
    if (!status.IsKeyNotExist()) {
        exit(0);
        cout << "after del, get key: " 
            << KEY(key) << " should not success" << endl;
        return false;
    }
    return true;
}

std::string Test::Hex(const std::string &str)
{
    std::ostringstream o;
    o << std::hex;
    std::string::const_iterator it = str.begin();
    for (; it != str.end(); it++) {
        o << *it;
    }

    return o.str();
}

#define NON_LEATER " ~ ` ! @ # $ % ^ & * ( ) | \\ } ] { [ \" ' : ; ? / > . < , "

bool Test::Run()
{
    //text test
    std::string key("hello world");
    std::string value("hello world");
    ASSERT_TRUE(Basic(key, value));
    
    key = NON_LEATER;
    value = NON_LEATER;
    ASSERT_TRUE(Basic(key, value));

    //binary test
    //key len: 1*256, value len: 1*256
    ASSERT_TRUE(Binary(1, 1));
    //key len: 1K, value len: 10K
    ASSERT_TRUE(Binary(1*1024/256, 10*1024/256));

    //key len: 60K, value len: 1M
    ASSERT_TRUE(Binary(60*1024/256, 1*1024*1024/256)); 
    //key len: 60K, value len: 5M
    ASSERT_TRUE(Binary(60*1024/256, 5*1024*1024/256)); 
    //key len: 60K, value len: 10M
    ASSERT_TRUE(Binary(60*1024/256, 10*1024*1024/256)); 
    //key len: 60K, value len: 20M
    ASSERT_TRUE(Binary(60*1024/256, 20*1024*1024/256)); 
#if 0
    //batch test
    //ASSERT_TRUE(Batch(10000, 1*1024/256, 10*1024/256));
    //ASSERT_TRUE(Batch(1000000, 1*1024/256, 10*1024/256));
#endif
    
    cout << "Run Ok" << endl;

    return true;
}
