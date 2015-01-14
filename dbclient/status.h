// status.h (2015-01-14)
// Yan Gaofeng (yangaofeng@360.cn)

#include <string>

class Status {
public:
    enum {
        kOk = 0,
        kKeyNotExist,
        kServerExit,
        kUnknown
    };

    Status() : code_(kOk) {}
    Status(int code) : code_(code) {}

    static Status Ok() { return Status(kOk); }
    static Status KeyNotExist() { return Status(kKeyNotExist); }
    static Status ServerExit() { return Status(kServerExit); }
    static Status Unknown() { return Status(kUnknown); }


    bool IsOk() { return code_ == kOk; }
    bool IsKeyNotExist() { return code_ == kKeyNotExist; }
    bool IsServerExit() { return code_ == kServerExit; }
    bool IsUnknown() { return code_ == kUnknown; }


    std::string ToString() {
        switch (code_) {
            case kOk:
                return "success";
            break;
            case kKeyNotExist:
                return "key not exist";
            break;
            case kServerExit:
                return "server exit";
            break;
            case kUnknown:
                return "unknown error";
            break;
        }

        return "unknown error code";
    }

private:
    int code_;
};
