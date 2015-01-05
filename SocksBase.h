#ifndef _SOCKSBASE_H_
#define _SOCKSBASE_H_

#include "generic.h"

#define  SOCKS_BUILD_TARGET_SOCKET_ERROR   -1

struct rcsocktul{
    char URL[300];
    int port;
    char poolnum;   
};


int socks_build_target_socket(int cmd_sock);
void *socks_check_and_tunnel(void *sock);

#endif
