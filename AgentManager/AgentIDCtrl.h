#ifndef _AGENTIDCTRL_H_
#define _AGENTIDCTRL_H_

#include "../generic.h"
#include "GlobalValue.h"
#include "AgentProtocol.h"
// 申请新ID
#define AGENT_ID_ASK_ERROR  -1
int AGENT_ID_ASK();

// 当有新ID申请请求时
#define ON_AGENTID_ASK_ERROR  -1
#define ON_AGENTID_ASK_OK      1
int on_AGENT_ID_Ask(pAgent_proto proto,pPCConn conn);

#endif
