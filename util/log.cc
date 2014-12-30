// log.cc (2014-12-27)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

#include "log.h"

namespace ldb {
namespace util {

Log *info_log = NULL;
Log *error_log = NULL;

Log::Log(int fd, std::string logfile_path, bool is_thread_safe):
    fd_(fd), logfile_path_(logfile_path), 
    is_thread_safe_(is_thread_safe), mutex_(NULL), current_size_(0)
{
    if(is_thread_safe_) {
        ThreadSafe();
    }
}

Log::~Log()
{
    if(mutex_){
        pthread_mutex_destroy(mutex_);
        free(mutex_);
    }
    
    Close();
}

void Log::Close()
{
    if (fd_ > 0) close(fd_);
    fd_ = -1;
}

void Log::ThreadSafe()
{
    mutex_ = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex_, NULL);
}


int Log::Write(const char *fmt, va_list ap)
{
    char buf[2048];
    int len;
    char *ptr = buf;

    time_t time;
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    tm = localtime(&time);

    len = sprintf(ptr, "%04d-%02d-%02d %02d:%02d:%02d.%03d ",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec/1000));
    if(len < 0){
        return -1;
    }
    ptr += len;

    int space = sizeof(buf) - (ptr - buf) - 10;
    len = vsnprintf(ptr, space, fmt, ap);
    if(len < 0){
        return -1;
    }
    ptr += len > space? space : len;
    *ptr++ = '\n';
    *ptr = '\0';

    len = ptr - buf;
   
    if(mutex_){
        pthread_mutex_lock(mutex_);
    }

    if (write(fd_, buf, len) < 0) {
        //error
        return -1;
    }

    current_size_ += len;
    if (current_size_ > rotate_size_) {
       if (Rotate() < 0) {
           return -1;
           //error
       }
    }

    if(mutex_){
        pthread_mutex_unlock(mutex_);
    }

    return len;
}

int Log::Rotate()
{
    Close();
    
    char newpath[1024];
    time_t time;
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    tm = localtime(&time);
    sprintf(newpath, "%s.%04d%02d%02d-%02d%02d%02d",
            logfile_path_.c_str(),
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    int ret = rename(logfile_path_.c_str(), newpath);
    if (ret == -1) {
        return -1;
    }

    fd_ = open(logfile_path_.c_str(), O_RDWR | O_CREAT | O_APPEND);
    if (fd_ == -1) {
        return -1; 
    }

    current_size_ = 0;

    return 0;
}

int Log::LogWrite(const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    int ret = Write(fmt, ap);
    if (ret < 0) {
        return -1;
    }
    
    va_end(ap);
    return ret;
}

} /*namespace util*/
} /*namespace ldb*/

