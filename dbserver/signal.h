// signal.h (2015-01-27)
// WangPeng (1245268612@qq.com)

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <dbserver/server.h>

extern bool quit;
extern Server server;
extern char *arg;

void SigProcess();
static void SigtermHandler(int sig);
static void SigChild(int signo);

#endif
