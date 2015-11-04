#ifndef _PC_NODE_INFO_H_
#define _PC_NODE_INFO_H_

#include "../generic.h"

#define MAX_PCNAME_LEN     50
#define MAX_IP_ADDR_LEN    20

// first conn send msg
#define IAM_ADMIN_NODE      1
#define MYSELF_NODE         2
#define WITH_ADMIN_NODE     3
// other conn msg
#define UPSTREAM_NODE       4
#define NEIGHBOR_NODE       5
#define REMOTION_NODE       6
#define UNKONWN_NODE        7
typedef struct PCConnInfo{
    int LinkType;
    char IPaddr[MAX_IP_ADDR_LEN];
    int port;
    int cmd_socket;
}PCConn,*pPCConn;

#define PC_OS_LINUX        1
#define PC_OS_WINDOW       2
#define PC_OS_ARMLINUX     3
#define PC_OS_MACOSX       4
#define PC_OS_UNKNOW       5
typedef struct PCNodeData{
    int id;
    int OSType; // OS_Type // 
    char PCName[MAX_PCNAME_LEN];
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
    int LinkType,
    char *ipaddr,
    int port,
    int cmd_socket
);

void PCNODE_Free(pPCNodeInfo node);
pPCNodeInfo PCNODE_Copy(pPCNodeInfo node);

#endif
