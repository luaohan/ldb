// log.h (2014-12-27)
// WangPeng (1245268612@qq.com)

#ifndef _LOG_H_
#define _LOG_H_

#include <string>
#include <stdarg.h>
#include <pthread.h>

class Log {

    public:
        Log(int fd, std::string logfile_path, bool is_thread_safe);
        ~Log();

        int LogWrite(const char *fmt, ...);
        void Close();

    private:
        int Write(const char *fmt, va_list ap);
        void ThreadSafe();
        int Rotate();

    private:

        int fd_;
        std::string logfile_path_;
        
        bool is_thread_safe_;
        pthread_mutex_t *mutex_;
        
        int current_size_;

        static const int rotate_size_ = 1024 * 1024 * 10;// 10M
};

#define log_info(fmt, args...) if (info_log != NULL) info_log->LogWrite(fmt, ##args)
#define log_error(fmt, args...) if (error_log != NULL) error_log->LogWrite(fmt, ##args)


#endif
