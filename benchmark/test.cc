// boundary.cc (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

#include <assert.h>
#include <iostream>
#include <sstream>
#include <dbclient/ldbc.h>
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

bool Test::Basic(std::string &key, std::string &value)
{
    cout << "Enter Basic test" << endl;

    mod_ = kText;

    bool ret = Exec(key, value);

    cout << "Leave Basic test" << endl;

    return ret;
}

bool Test::Binary(int key_len, int value_len)
{
    cout << "Enter Binary, test" << endl;

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

    bool ret = Exec(key, value);

    cout << "Leave Binary, test" << endl;

    return ret;
}

bool Test::Batch(int count, int key_len, int value_len)
{
    cout << "Enter Batch, test" << endl;

    mod_ = kBinary;

    for (int i = 0; i < count; i++) {
        bool ret = Binary(key_len, value_len);
        if (!ret) {
            return false;
        }
    }

    cout << "Leave Batch, test" << endl;

    return true;
}

#define KEY(k) (mod_ == kText ? k : Hex(k))

bool Test::Exec(std::string &key, std::string &value)
{
    //cout << "test mode: " 
    //    << mod_ == kText ? "text" : "binary"  << endl;
    //cout << "key size: " << key.size() 
    //    << ", value size: " << value.size() << endl;

    int rc = client_->Set(key, value);
    if (rc == -1) {
        cout << "set key: " << KEY(key) << " failed" << endl;
        return false;
    }

    std::string response;
    rc = client_->Get(key, &response);
    if (rc == -1) {
        cout << "get key: " << KEY(key) << " failed" << endl;
        return false;
    }
    if (response != value) {
        cout << "get key: " << KEY(key) 
            << " failed, invalid result: "
            << response << endl;
        return false;
    }

    rc = client_->Del(key);
    if (rc == -1) {
        cout << "del key: " << KEY(key) << " failed" << endl;
        return false;
    }

    rc = client_->Get(key, &response);
    if (rc != -1) {
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
    //key len: 64K, value len: 2M
    ASSERT_TRUE(Binary(64*1024/256, 2*1024*1024/256)); 
    //key len: 500K, value len: 5M
    ASSERT_FALSE(Binary(500*1024/256, 5*1024*1024/256)); 

    //batch test
    ASSERT_TRUE(Batch(10000, 1*1024/256, 10*1024/256));
    //ASSERT_TRUE(Batch(1000000, 1*1024/256, 10*1024/256));

    return true;
}
