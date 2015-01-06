#ifndef _LCX_BASE_H_
#define _LCX_BASE_H_
#include"generic.h"

int lcx_listen(int from_port,int cmd_port,int usec);
int lcx_tran(int from_port,char *to_host,int to_port,int usec);
int lcx_slave(char *from_host,int from_port,char *to_host,int to_port,int usec);

#endif
