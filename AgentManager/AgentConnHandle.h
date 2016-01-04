#ifndef _AGENTCONNHANDLE_H_
#define _AGENTCONNHANDLE_H_

#include "../generic.h"
#include "GlobalValue.h"
#include "AgentProNet.h"


// send Myself Info and Update PCNodeManager
#define AGENTCONN_INTERACTIVE_ERROR   -1
#define AGENTCONN_INTERACTIVE_OK       1
int AgentInfo_Interactive(int sock,char *ip,int port);


int on_Agent_Connect(int sock);


#define ON_NEWAGENT_CONNECT_ERROR  -1
#define ON_NEWAGENT_CONNECT_OK      1
int on_NewAgent_Connect(pAgent_proto proto,pPCConn conn);

#endif
