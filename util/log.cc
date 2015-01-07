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
#include <assert.h>

#include "log.h"

Log *log = NULL;

Log::Log(std::string logfile_path, int level, int rotate_size):
    fd_(-1), logfile_path_(logfile_path), level_(level),
    current_size_(0), rotate_size_(rotate_size)
{
    if (rotate_size_ < 1024 * 1024 * 10) {
        rotate_size_ = 1024 * 1024 * 10; //10M
    }

    //printf(":%s\n", logfile_path_.c_str());

    fd_ = open(logfile_path_.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

    //assert(fd_ != -1);
}

#if 0
Log::HasDir(const char *str) 
{
    int pfrond = 0;
    int pbehind = strlen(str) - 1;
    if (strpfrond)
}
#endif

Log::~Log()
{
    Close();
}

void Log::Close()
{
    if (fd_ > 0) {
        close(fd_);
        fd_ = -1;
    }
}

const char *Log::LevelString(int level)
{
    assert(level >= 0 && level <= 4);
   
    switch(level){
        case 0:
            return "[FATAL] ";
        case 1:
            return "[ERROR] ";
        case 2:
            return "[WARN ] ";
        case 3:
            return "[INFO ] ";
        case 4:
            return "[DEBUG] ";
    }
    
    return "";
}

int Log::Write(int level, const char *fmt, va_list ap)
{
    if (level_ < level) {
        return 0;
    }
    
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
   
    memcpy(ptr, LevelString(level), 8);
    ptr += 8;
   
    int space = sizeof(buf) - (ptr - buf) - 10;
    len = vsnprintf(ptr, space, fmt, ap);
    if(len < 0){
        return -1;
    }
   
    ptr += len > space? space : len;
    *ptr++ = '\n';
    *ptr = '\0';
    len = ptr - buf;
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
    sprintf(newpath, "%s.%04d%02d%02d-%02d%02d%02d-%03d",
            logfile_path_.c_str(),
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec/1000));

    int ret = rename(logfile_path_.c_str(), newpath);
    if (ret == -1) {
        return -1;
    }
   
    fd_ = open(logfile_path_.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd_ == -1) {
        return -1;
    }
   
    current_size_ = 0;
  
    return 0;
}

int Log::LogWrite(int level, const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int ret = Write(level, fmt, ap);
    if (ret < 0) {
        return -1;
    }
   
    va_end(ap);
   
    return ret;
}
