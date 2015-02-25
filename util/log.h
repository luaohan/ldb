// log.h (2014-12-27)
// WangPeng (1245268612@qq.com)

#ifndef _LOG_H_
#define _LOG_H_

#include <string>
#include <stdarg.h>

class Log {
public:
    Log(std::string logfile_path, int level, int rotate_size = 1024 * 1024 * 10);
    ~Log();

    int LogWrite(int level, const char *fmt, ...);
    void Close();

public:
    enum {
        LEVEL_FATAL = 0,
        LEVEL_ERROR,
        LEVEL_WARN,
        LEVEL_INFO,
        LEVEL_DEBUG
    };

private:
    int Write(int level, const char *fmt, va_list ap);
    int Rotate();
    const char *LevelString(int level);

    void Mkdir(const char *str);

private:
    int fd_;
    std::string logfile_path_;
    int level_;
    int current_size_;
    int rotate_size_ ;
};

extern Log *log;

#define log_fatal(fmt, args...) if (log != NULL) \
            log->LogWrite(0, "%s(%d)" fmt, __FILE__, __LINE__, ##args)

#define log_error(fmt, args...) if (log != NULL) \
            log->LogWrite(1, "%s(%d)" fmt, __FILE__, __LINE__, ##args)

#define log_warn(fmt, args...) if (log != NULL) \
            log->LogWrite(2, "%s(%d)" fmt, __FILE__, __LINE__, ##args)

#define log_info(fmt, args...) if (log != NULL) \
            log->LogWrite(3, "%s(%d)" fmt, __FILE__, __LINE__, ##args)

#define log_debug(fmt, args...) if (log != NULL) \
            log->LogWrite(4, "%s(%d)" fmt, __FILE__, __LINE__, ##args)


#endif

