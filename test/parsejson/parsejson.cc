// parsejson.cc (2015-01-15)
// WangPeng (1245268612@qq.com)

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include "parsejson.h"

int GetNodeInfo(std::vector<Socket *> &server, const char *file_name)
{
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        return -1;
    }
    
    char buf[BUFSIZE] = {0};
    int ret = fread(buf, BUFSIZE, 1, fp);
    if (ret == 0) {
        if (errno != 0) {
            return -1;
        }
    }

    //可以有10 个真实主机，每个真实主机允许100个虚拟主机
    int number[10][100]; /* 0 1 2 -1...
                            3 4 5 -1...
                            6 7 8 -1...
                            -1 -1 -1...
                            ........... */
    for (int i = 0; i < 10; i++) {
        for(int j = 0; j < 100; j++) {
            number[i][j] = -1;
        }
    }
    
    int i = 0;
    int j = 0;
    // for num 
    while (buf[i] != '\0') {
        int p1, p2;

        while (buf[i] != '[') {
            if (buf[i] == '\0') {
                goto here;
            }
            i++;
        }
        p1 = ++i;

        while (buf[i] != ']') {
            if (buf[i] == '\0') {
                goto here;
            }
            i++;
        }
        p2 = ++i;
        
        char str[200];  //放数字串，如:1, 2, 3, 4, ...... 
        int len = p2 - p1 - 1;
        memcpy(str, buf + p1, len);
        str[len] = ' ';
        str[len + 1] = '\0';
        
        int m = 0;
        int n = 0;
        int q = 0;
        while (str[m] != '\0') {
            if (isxdigit(str[m])) {
                m += 1;
                continue;
            }
            if (m == n) {
                break;
            }
            char num[5]; //放数字
            memcpy(num, str + n, m - n);
            num[m-n] = '\0';
            if (num[0] != ','){
                number[j][q++] = atoi(num);
            }
            n = m;
            m += 1;
        }
        j++;
    }

here:
    i = 0;
    int x = 0;
    //for ip
    while (buf[i] != '\0') {
        int p1, p2;

        while (buf[i] != '\"') {
            if (buf[i] == '\0') {
                goto end;
            }
            i++;
        }
        
        p1 = ++i;

        while (buf[i] != '\"') {
            if (buf[i] == '\0') {
                goto end;
            }
            i++;
        }
        p2 = ++i;
        
        int len = p2 - p1 - 1;
        char ip[16];
        memcpy(ip, buf + p1, len);
        ip[len] = '\0';
        Socket *s = new Socket(ip);

        int y = 0;
        while (number[x][y] != -1) {
            int n = number[x][y];
            //server[n] = s; 
            server.push_back(s); 
            y++;
        }
        x++;
    }

end:
    fclose(fp);

    return 0;
}
