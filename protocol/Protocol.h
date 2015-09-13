#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include "PROTOCOL_FORMAT.h"

int PRO_start_server(int port,int maxline);
int PRO_socket_send(int sock,char *buffer,int buflen);
int PRO_socket_recv(int sock,char *buf,int maxlen);

#endif
