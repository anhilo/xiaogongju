#ifndef _SOCKSBASE_H_
#define _SOCKSBASE_H_

#define  SOCKS_BUILD_TARGET_SOCKET_ERROR   -1
int socks_build_target_socket(int cmd_sock);
void *socks_check_and_tunnel(void *sock);

#endif
