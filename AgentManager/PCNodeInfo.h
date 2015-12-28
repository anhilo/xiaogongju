#ifndef _PC_NODE_INFO_H_
#define _PC_NODE_INFO_H_

#include "../generic.h"
#include "AgentProNet.h"

#define MAX_PCNAME_LEN     50

#define PC_OS_LINUX        1
#define PC_OS_WINDOW       2
#define PC_OS_ARMLINUX     3
#define PC_OS_MACOSX       4
#define PC_OS_UNKNOW       5

// NodeType 
#define IAM_ADMIN_NODE      1 // Neighbor
#define MYSELF_NODE         2 // Neighbor
#define UPSTREAM_NODE       3 // REMOTE_CHILD
#define MYSELF_NODE_REMOTE  4 // REMOTE_CHILD
#define UNKONWN_NODE        5 // UNKNOWN
typedef struct PCNodeData{
    int id;
    int OSType; // OS_Type // 
    char PCName[MAX_PCNAME_LEN];
    int NodeType ;
    PCConn conn;
}PCNodeInfo,*pPCNodeInfo;


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
