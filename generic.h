#ifndef _GENERIC_H_
#define _GENERIC_H_

#define PTW32_STATIC_LIB

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<getopt.h>    //strlen

#ifdef WIN32
	#include<winsock2.h>
	#include<ws2tcpip.h>
	#include<unistd.h>
    #include<windows.h>
#else
    #include<pthread.h> //for threading , link with lpthread
	#include<netdb.h>
	#include<sys/socket.h>
	#include<arpa/inet.h>
	#include<signal.h>
#endif // head files check

// my headers start from here
#include "baselib/BaseAPI.h"
#include "CMD_Protocol.h"
#include "sockslib/SocksBase.h"
#include "sockslib/Sock_Tunnel.h"
#include "sockslib/ssocksd_pro.h"
#include "lcxlib/Lcx_Base.h"
#include "sockslib/rssocks_pro.h"
// end my headers


#ifdef WIN32
    #define MIC_THREAD_FUN_DEF(fun_name,parm) \
        WINAPI DWORD fun_name(LPVOID parm)
    #define MIC_THREAD_HANDLE_ID HANDLE
    #define MIC_THREAD_CREATE(handle,fun_name,parm) \
        CreateThread(NULL,0,fun_name,(void*)parm,0,NULL)
    #define MIC_THREAD_JOIN(handle) \
        Sleep(1)
    #define MIC_THREAD_END() Sleep(1)
    #define MIC_SLEEP(x)  Sleep(x*1000)
    #define MIC_USLEEP(x) Sleep(x);
#else
    #define MIC_THREAD_FUN_DEF(fun_name,parm) \
        void *fun_name(void *parm)
    #define MIC_THREAD_HANDLE_ID pthread_t
    #define MIC_THREAD_CREATE(handle ,fun_name,parm) \
        pthread_create(&handle,NULL,fun_name,(void*)parm)
    #define MIC_THREAD_JOIN(handle) \
        pthread_join(handle ,NULL)
    #define MIC_THREAD_END() pthread_detach(pthread_self())
    #define MIC_SLEEP(x)  sleep(x)
    #define MIC_USLEEP(x) usleep(x*1000);
#endif


#define Printf_Error(...) \
        (printf("[ ERROR ]  "),printf(__VA_ARGS__),printf("\n"))
#define Printf_OK(...)    \
        (printf("[ OK    ]  "),printf(__VA_ARGS__),printf("\n"))
#define Printf_DEBUG(...) \
        (printf("[ DEBUG ]  "),printf(__VA_ARGS__),printf("\n"))

#define MyPrintf(...) \
        (printf(__VA_ARGS__),printf("\n"))


#define MAX_POOL       1000
#define True              1
#define False             0
#define TUNMAX_BUF    10000


#endif // _GENERIC_H_
