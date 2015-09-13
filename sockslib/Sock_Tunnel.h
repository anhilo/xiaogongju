#ifndef _SockTunnel_H_
#define _SockTunnel_H_

#include "../generic.h"

#define TUNNEL_INIT_OK                           1
#define TUNNEL_INIT_FALSE                        0
#define TUNNEL_CLEAN_OK                          1
#define TUNNEL_CLEAN_FALSE                       0
#define TUNNEL_CLOSE_OK                          1
#define TUNNEL_CLOSE_FALSE                       0
#define TUNNEL_GET_POOL_ID_FALSE                -1
#define TUNNEL_GET_FIRST_POOL_ID_FALSE          -1
#define TUNNEL_SET_SECOND_POOL_AND_RUN_IT_OK     1
#define TUNNEL_SET_SECOND_POOL_AND_RUN_IT_FALSE -1
#define TUNNEL_SOCK_TO_SOCK_OK                   1
#define TUNNEL_SOCK_TO_SOCK_ERROR               -1

#define NULL_SOCK         0
#define SOCK_1_OK         1
#define SOCK_2_OK         2
#define POOL_WORKING_NOW  3
#define POOL_STOP_NOW     4


typedef struct sspool{
    int sock_1;
    int sock_2;
    int flag;
    int state;
    int usec_time;
}sockbind_s;

int tunn_init_Pool();
int tunn_clean(int num);
int tunn_close(int num);
//MIC_THREAD_FUN_DEF(tunn_run_now,p_num) ;
//void * tunn_run_now(void *p_num);
int tunn_get_pool_id_and_lock_it();
int tunn_set_first_pool_and_lock_it(int sock,int usec);
int tunn_set_second_pool_and_run_it(int pool_num,int sock);
int tunn_sock_to_sock(int from_sock,int to_sock,int usec);


#endif
