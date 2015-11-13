#ifndef _AGENTSOCK_H_
#define _AGENTSOCK_H_

#include"../generic.h"
#include "PCNodeManager.h"
#include "PCNodeInfo.h"
#include "AgentInteractive.h"


#define AGENTINFO_SEND_ERROR  -1
#define AGENTINFO_SEND_OK      1
int AgentInfo_Send(int sock,pPCNodeInfo info);

#define AGENTINFO_RECV_ERROR  NULL
pPCNodeInfo AgentInfo_Recv(int sock);

#define AGENTINFO_SETID_ERROR  -1
#define AGENTINFO_SETID_OK      1
int AgentInfo_SetID(int sock,int id);

#define AGENTINFO_RECVID_ERROR -1
int AgentInfo_RecvID(int sock);

int AgentInfo_SendNTH(int sock);
#endif
