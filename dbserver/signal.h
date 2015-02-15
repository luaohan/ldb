// signal.h (2015-01-27)
// WangPeng (1245268612@qq.com)

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <dbserver/server.h>
#include <util/log.h>

extern Server *server;
extern pid_t father_id;

void SigProcess();
static void SigTermHandler(int sig);
static void SigChildHandler(int signo);


#endif
