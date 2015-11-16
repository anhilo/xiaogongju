#ifndef _AGENTCONNHANDLE_H_
#define _AGENTCONNHANDLE_H_

#include "../generic.h"

// send Myself Info and Update PCNodeManager
#define AGENTCONN_INTERACTIVE_ERROR   -1
#define AGENTCONN_INTERACTIVE_OK       1
int AgentInfo_Interactive(int sock);


int on_Agent_Connect(int sock);
#endif
