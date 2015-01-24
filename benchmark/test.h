// test.h (2015-01-09)
// Yan Gaofeng (yangaofeng@360.cn)

class Client;

class Test {
public:
    Test(Client *client);
    ~Test() {}

    bool Run();

private:
    enum {
        kText = 0,
        kBinary
    };

    bool Exec(std::string &key, std::string &value, bool only_set = false);
    bool Basic(std::string &key, std::string &value, bool only_set = false);
    bool Binary(int key_len, int value_len, bool only_set = false);
    bool Batch(int count, int key_len, int value_len);

    std::string Hex(const std::string &str);

private:
    Client *client_;
    int mod_;
    unsigned char bin_[256];
};

