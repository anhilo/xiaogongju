#ifndef _BASEAPI_H_
#define _BASEAPI_H_

#include "generic.h"


#define ENV_INIT_OK                1
#define ENV_INIT_FALSE             0
#define SOCKET_CONNECT_ERROR      -1
#define SOCKET_SERVER_INIT_ERROR  -1
#define SOCKET_SEND_ERROR         -1
#define SOCKET_RECV_ERROR         -1
#define SOCKET_INIT_ERROR         -1
#define SOCKET_OVER_TIME           0
#define SOCKET_MAY_CLOSE_NOW       1
#define SOCKET_CAN_READ_STATE      2
#define SOCKET_CAN_WRITE_STATE     2
#define I_TO_CHAR_ERROR           -1
#define I_TO_CHAR_OK               1
#define CHAR_TO_I_ERROR           -1


int API_env_init();

// api for socket
int API_socket_connect(char *url,int port);
int API_socket_init_server(int port,int maxlisten);
int API_socket_send(int sock,char *buf,int buflen);
int API_socket_recv(int sock,char *buf,int buflen);
int API_socket_close(int sock);
int API_socket_read_state(int sock,int sec,int usec);
int API_socket_write_state(int sock,int sec,int usec);

struct hostent *API_socket_gethostbyname(char * ser_addr);

//  API_getsock_opt(int sock);

// api for thread
int API_thread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);
int API_thread_detach();

// api for math
int API_m_itochar(int a,char *b,int len);
    // #define I_TO_CHAR_ERROR  -1
    // #define I_TO_CHAR_OK     1
    // 如果 返回OK  表明 char * b 是最终结果。
int API_m_chartoi(char *b,int len);
    // #define CHAR_TO_I_ERROR  -1
    // 如果不是错误那么结果就是正确的
// api for usec times and sleep
int API_m_sleep(int usec);
int API_set_usec_time(int usec);
int API_get_usec_time();

#endif
