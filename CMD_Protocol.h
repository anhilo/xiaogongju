#ifndef _CMD_PROTO_H_
#define _CMD_PROTO_H_


#define PROTO_INIT_CMD_SER_ERROR        0
#define PROTO_INIT_CMD_SER_OK           1
#define PROTO_INIT_CMD_RCSOCKET_ERROR  -1
#define PROTO_GET_RCSOCKET_ERROR       -1
#define PROTO_UNDERSTAND_ERROR         -2
#define PROTO_UNDERSTAND_BUT_DO_ERROR  -1
#define proto_UNDERSTAND_AND_DO_OK      1 
#define PRO_SEND_RCCMD_ERROR           -1
#define PRO_SEND_RCCMD_OK               1

#define UNKOWN_MSG                  0
#define PRO_MAKE_NET                1

 #define READ_ERROR_PLEASE_SAY_AGAIN 0   // 服务器没理解，重说一遍
 #define I_AM_NEW_RC_CMD_SOCK_CLIENT 1   // 新主机反向连接请求
 #define CONFIRM_YOU_ARE_SOCK_CLIENT 2   // 新主机连接确认
 #define I_AM_NEW_RC_SOCK_FOR_TUNNEL 3   // 新隧道反向连接请求
        // 新隧道要带 POOLNUM 
#define CONFIRM_YOU_ARE_SOCK_TUNNEL 4   // 新隧道连接确认

#define PRO_BROAD_MSG               2

int proto_init_cmd_server_for_rc(int port);
int proto_init_cmd_rcsocket(char *URL,int port);
int proto_get_rcsocket(char *URL,int port,int poolnum);
int proto_understand_and_do_it(int sock);
int proto_send_rccmd_poolnum(int sock,int poolnum);


#endif
