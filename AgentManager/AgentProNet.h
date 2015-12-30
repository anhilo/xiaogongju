#ifndef _AGENTPRONET_H_
#define _AGENTPRONET_H_

#include "../generic.h"

#define MAX_IP_ADDR_LEN    20

#define CONNTYPE_DIRECT_CONNECT   1
#define CONNTYPE_REVERSE_CONNECT  2
#define NET_SESSION_BUSY_NOW      1
#define NET_SESSION_UNUSED_NOW    2
typedef struct PCConnInfo{
    int ConnType;
    int BusyType;   // BusyType
    char IPaddr[MAX_IP_ADDR_LEN];
    int port;
    int cmd_socket;
}PCConn,*pPCConn;

#define PCCONN_CREATEPCCONNFROMSOCKET_ERROR  NULL
pPCConn PCCONN_CreatePCConnFromSocket(int sock);

#define PCCONN_COPY_ERROR           NULL
pPCConn PCCONN_Copy(pPCConn conn); 

#define PCCONN_FREE_ERROR        0
#define PCCONN_FREE_OK           1
int PCCONN_Free(pPCConn conn);


#define PROTO_SENDPROTO_ERROR         -1
#define PROTO_SENDPROTO_OK             1
int PROTO_SendProto(pPCConn conn,pAgent_proto proto);

#define PROTO_RECVSTATE_CANRECV        1
#define PROTO_RECVSTATE_SOCKET_ERROR   2
#define PROTO_RECVSTATE_OVERTIME       3
int PROTO_RecvState(pPCConn conn);

#define PROTO_GETPROTO_ERROR    0 //出现错误
#define PROTO_GETPROTO_OK       1 //接受消息成功
#define PROTO_GETPROTO_NOTHING  2 //未收到消息
int PROTO_RecvProto(pPCConn conn,pAgent_proto proto);

#endif
