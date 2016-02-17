#ifndef _AGENTPRONET_H_
#define _AGENTPRONET_H_

#include "../generic.h"
#include "GlobalValue.h"
#include "AgentProtocol.h"
#include "PCNodeInfo.h"
#include "PCConn.h"

#define PROTO_SENDPROTO_ERROR         -1
#define PROTO_SENDPROTO_OK             1
int PROTO_SendProto(pPCConn conn,pAgent_proto proto);

#define PROTO_RECVSTATE_CANRECV        1
#define PROTO_RECVSTATE_SOCKET_ERROR   2
#define PROTO_RECVSTATE_OVERTIME       3
int PROTO_RecvState(pPCConn conn,int sec,int usec);

#define PROTO_RECVPROTO_ERROR       NULL
pAgent_proto PROTO_RecvProto(pPCConn conn);

#define PROTO_SENDPCNODEINFO_ERROR     0
#define PROTO_SENDPCNODEINFO_OK        1
int PROTO_SendPCNodeInfo(pPCConn conn,pPCNodeInfo info);

#define PROTO_ANALYSISPCNODEINFO_ERROR   NULL
pPCNodeInfo PROTO_AnalysisPCNodeInfo(pPCConn conn,pAgent_proto proto);

#endif
