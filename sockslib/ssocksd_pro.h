#ifndef _SSOCKSD_H_
#define _SSOCKSD_H_

#include "../generic.h"

#define CREATE_SSOCKSD_SERVER_ERROR -1
#define CREATE_SSOCKSD_SERVER_OK     0

int create_ssocksd_server(int port,int usec);

#endif
