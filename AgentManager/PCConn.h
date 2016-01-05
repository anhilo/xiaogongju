#ifndef _PCCONN_H_
#define _PCCONN_H_

#include "../generic.h"
#include "GlobalValue.h"
#include "AgentProtocol.h"
#define PCCONN_CREATEPCCONNFROMSOCKET_ERROR  NULL
pPCConn PCCONN_CreatePCConnFromSocket(int sock);

#define PCCONN_CREATEPCCONNFROMIPPORT_ERROR NULL
pPCConn PCCONN_CreatePCConnFromIPPort(char *ip,int port,int sock);

#define PCCONN_COPY_ERROR           NULL
pPCConn PCCONN_Copy(pPCConn conn); 


#define PCCONN_SENDDATA_ERROR   -1
int PCCONN_SendData(pPCConn conn,char *data,int datalen);

#define PCCONN_RECVDATA_ERROR   -1
int PCCONN_RecvData(pPCConn conn,char *data,int maxlen);

#define PCCONN_FREE_ERROR              0
#define PCCONN_FREE_OK                 1
int PCCONN_Free(pPCConn conn);
#endif
