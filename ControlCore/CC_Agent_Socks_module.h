#ifndef _CC_AGENT_SOCKS_MODULE_H_
#define _CC_AGENT_SOCKS_MODULE_H_

#include "../generic.h"
#include "../AgentManager/PCConn.h"

#define CC_STARTSOCKS_ERROR   -1
#define CC_STARTSOCKS_OK       1
int CC_StartSocks(int targetid,int lport,int usec);


int on_CC_startSocks(pPCConn conn);

#endif
