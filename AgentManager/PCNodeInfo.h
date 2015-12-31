#ifndef _PC_NODE_INFO_H_
#define _PC_NODE_INFO_H_

#include "../generic.h"
#include "AgentProNet.h"


pPCNodeInfo PCNODE_Create();

#define PCNODE_SETALLDATA_ERROR  -1
#define PCNODE_SETALLDATA_OK      1
int PCNODE_SETAllData(
    pPCNodeInfo node,
    int id,
    int OSType,
    char *PCName,
    int NodeType,
    int ConnType,
    char *ipaddr,
    int port,
    int cmd_socket
);

void PCNODE_Free(pPCNodeInfo node);
pPCNodeInfo PCNODE_Copy(pPCNodeInfo node);

#endif
