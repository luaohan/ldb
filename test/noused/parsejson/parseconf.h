// parsejson.h (2015-01-15)
// WangPeng (1245268612@qq.com)

#ifndef _PARSONJSON_H_
#define _PARSONJSON_H_

#include <vector>

#include "../../net/socket.h"

#define BUFSIZE  1024 * 100

int GetNodeInfo(std::vector<Socket *> &servers, const char *file_name);

#endif
