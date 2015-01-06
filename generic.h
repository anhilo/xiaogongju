#ifndef _GENERIC_H_
#define _GENERIC_H_

#define PTW32_STATIC_LIB

#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<getopt.h>    //strlen
#include<pthread.h> //for threading , link with lpthread

// my headers start from here
#include "BaseAPI.h"
#include "CMD_Protocol.h"
#include "SocksBase.h"
#include "Sock_Tunnel.h"
#include "ssocksd_pro.h"
#include "Lcx_Base.h"
#include "rssocks_pro.h"
// end my headers

#ifdef WIN32
	#include<winsock2.h>
	#include<ws2tcpip.h>
	#include<unistd.h>
#else
	#include <netdb.h>
	#include<sys/socket.h>
	#include<arpa/inet.h>
	#include<signal.h>
#endif // head files check

#define MAX_POOL       1000
#define True              1
#define False             0
#define TUNMAX_BUF    10000


#endif // _GENERIC_H_
