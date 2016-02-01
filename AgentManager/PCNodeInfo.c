#include "PCNodeInfo.h"

pPCNodeInfo PCNODE_Create(){
    pPCNodeInfo node = (pPCNodeInfo)malloc(sizeof(PCNodeInfo));
    int res = PCNODE_SETAllData(
        node,
        -1,
        PC_OS_UNKNOW,
        "No Name Now",
        -1,
        UNKONWN_NODE,
        "no ip",
        -1,
        -1
    );
    if(res == PCNODE_SETALLDATA_ERROR){
        PCNODE_Free(node);
        node = NULL;
        return NULL;
    }
    return node;
}

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
){
    if(node == NULL 
        || strlen(PCName)  > (unsigned long)MAX_PCNAME_LEN
        || strlen(ipaddr)  > (unsigned long)MAX_IP_ADDR_LEN
    ){
        return PCNODE_SETALLDATA_ERROR;
    }
    node -> id = id;
    node -> id = id;
    node -> OSType = OSType;
    strncpy( node-> PCName , PCName,(unsigned long) MAX_PCNAME_LEN );
    node -> NodeType = NodeType;
    node -> conn.ConnType = ConnType;
    strncpy( node-> conn.IPaddr, 
        ipaddr , (unsigned long)MAX_IP_ADDR_LEN);
    node -> conn.port = port;
    node -> conn.cmd_socket = cmd_socket;
    node -> conn.BusyType   = NET_SESSION_UNUSED_NOW;
    return PCNODE_SETALLDATA_OK;
}

void PCNODE_Free(pPCNodeInfo node){
    if( node == NULL ){
        return ;
    }
    free(node);
    return ;
}

pPCNodeInfo PCNODE_Copy(pPCNodeInfo node){
    pPCNodeInfo newnode = PCNODE_Create();
    int result = PCNODE_SETAllData(
        newnode,
        node -> id,
        node -> OSType,
        node -> PCName,
        node -> NodeType,
        node -> conn.ConnType,
        node -> conn.IPaddr,
        node -> conn.port,
        node -> conn.cmd_socket
    );
    if(result == PCNODE_SETALLDATA_ERROR){
        PCNODE_Free(newnode);
        return NULL;
    }
    return newnode;
}

