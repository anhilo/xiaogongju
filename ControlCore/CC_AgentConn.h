#ifndef _CC_AGENTCONN_H_
#define _CC_AGENTCONN_H_

#include "../generic.h"
#include "CCProxy.h"

int CC_Agent_Conn_Listen(int target,char *pcname,int node_type);

int CC_Agent_Conn_onListen(int sock);

int CC_Agent_Conn_connect(int target,char *

#endif
