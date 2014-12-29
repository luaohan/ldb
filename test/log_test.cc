// log_test.cc (2014-12-27)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "../log.h"

int main()
{
    int fd = open ("./log.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "open error: %s\n", strerror(errno));
        return -1;
    }

    std::string path = "./log.txt";

    Log loger(fd, path, false);
    for (int i = 0; i < 1000; i++) {
        loger.LogWrite("abcdefghijklmnopqrstuvwzyx%d\n", i);
    }

    return 0;
}
