/**
* File Name :AgentManager/AgentConnHandle.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 一 11/16 00:13:14 2015
*/
#include "AgentConnHandle.h"
#include "PCNodeManager.h"
#include "PCNodeInfo.h"
#include "AgentIDCtrl.h"
#include "AgentProNet.h"
#include "AgentCMDParse.h"
#include "AgentChildSync.h"


//====================================
pPCNodeInfo m_agentInfo_Recv(pPCConn conn);
int m_Info_send(pPCConn conn,pPCNodeInfo info);
#define M_INFO_SEND_ERROR     -1
#define M_INFO_SEND_OK         1
//int m_Info_send(int sock,pPCNodeInfo info);
#define M_INFO_RECV_ERROR   NULL
//pPCNodeInfo m_agentInfo_Recv(int sock);
int When_Iam_WithAdmin_Connect(pPCNodeInfo myself,pPCConn conn);
int When_Iam_Normal_Node(pPCNodeInfo myself,pPCConn conn);
//====================================

int m_AddNeighborProxy(pPCNodeInfo info){
    int res = PCMANAGER_ADDNeighbor(info);
    if(PCMANAGER_ADDNEIGHBOR_ID_CLASH == 
        res ){
        int newid = AGENT_ID_ASK();
        if(PCMANAGER_REPLACEID_ERROR == 
            PCMANAGER_ReplaceID(info->id,newid)){
            return 0;
        }
        if(PCMANAGER_ADDNEIGHBOR_OK ==
              PCMANAGER_ADDNeighbor(info)){
            return 1;
        }
    }
    else if(PCMANAGER_ADDNEIGHBOR_OK == res ){
        return 1;
    }
    Printf_Error("add error ????");
    return 0;
}

// be called By client
int AgentInfo_Interactive(int sock,char *ip,int port){
    pPCNodeInfo myself = PCMANAGER_Get_RootNode();
    pPCConn conn = PCCONN_CreatePCConnFromIPPort(ip,port,sock);
    if(myself == NULL || conn == NULL){
        return AGENTCONN_INTERACTIVE_ERROR;
    }
    if(PROTO_SENDPROTO_ERROR == 
        PROTO_SendPCNodeInfo(conn,myself)){
        return AGENTCONN_INTERACTIVE_ERROR;
    }
    /////// Add code From here
    m_Info_send(conn,myself);
    switch(myself->NodeType ){
    case  MYSELF_NODE:
        When_Iam_Normal_Node(myself,conn);
//        when_IAM_NORMALNODE(sock,ip,port);
        break;
    case IAM_ADMIN_NODE:
    case BE_MANAGED_NOW:
        When_Iam_WithAdmin_Connect(myself,conn);
        break;
    default:
        Printf_Error("NodeType is error");
        break;
    }
    return AGENTCONN_INTERACTIVE_OK;
}

//******************************************************************
int m_Info_send(pPCConn conn,pPCNodeInfo info){
    char buffer[300];
    char idbuf[4],ostype[4],nodetype[4];
    int  namelen;
    int  result = M_INFO_SEND_OK;

    if( conn == NULL || info == NULL ){
        result = M_INFO_SEND_ERROR;
    }
    else{
        namelen = strlen(info->PCName);
        if( namelen > MAX_PCNAME_LEN ){
            result = M_INFO_SEND_ERROR;
        }
        else{
            API_m_itochar (info->id       ,&(buffer[ 0]) ,4);
            API_m_itochar (info->OSType   ,&(buffer[ 4]) ,4);
            API_m_itochar (info->NodeType ,&(buffer[ 8]) ,4);
            API_m_itochar (namelen        ,&(buffer[12]) ,4);
            strncpy(&(buffer[16]), info->PCName,namelen+1);
           // send it  
            if( PCCONN_SENDDATA_ERROR == 
                PCCONN_SendData(conn,buffer,300)){
                result = M_INFO_SEND_ERROR;
            }
        }
    }
    if(result == M_INFO_SEND_ERROR){
        Printf_Error("Send info Error");
    }
    return  result;
}

#define BUF_ERROR   -1
#define BUF_OK       1
pPCNodeInfo m_agentInfo_Recv(pPCConn conn){
    char databuf[300];
    pPCNodeInfo info = PCNODE_Create();
    int id,ostype,nodetype,namelen;
    char pcname[MAX_PCNAME_LEN+1];
    int result = 1;
    if(info == NULL || conn == NULL){
        result = BUF_ERROR;
    }
    else{
        // recv id
        if(PCCONN_RECVDATA_ERROR == 
            PCCONN_RecvData(conn,databuf,300)){
            result = BUF_ERROR;
        }
        else{
            id       = API_m_chartoi(&(databuf[ 0]),4);
            ostype   = API_m_chartoi(&(databuf[ 4]),4);
            nodetype = API_m_chartoi(&(databuf[ 8]),4);
            namelen  = API_m_chartoi(&(databuf[12]),4);
            strncpy(pcname,&(databuf[16]),MAX_PCNAME_LEN+1);
            int res = PCNODE_SETAllData(
                info,
                id,
                ostype,
                pcname,
                nodetype,
                conn->ConnType,
                conn->IPaddr,
                conn->port,
                conn->cmd_socket
            );
            if(res == PCNODE_SETALLDATA_ERROR){
                result = BUF_ERROR;
            }
        }
    }
    if(result == BUF_ERROR){
        return M_INFO_RECV_ERROR;
    }
Printf_OK("[recv ]id = %d,ostype = %d , nodetype = %d, pcname = %s",
        info-> id,info->OSType,info->NodeType,info->PCName);
    return info;
}

//******************************************************************

#define WHEN_IAM_WITHADMIN_CONNECT_ERROR  -1
#define WHEN_IAM_WITHADMIN_CONNECT_OK      1
int When_Iam_WithAdmin_Connect(pPCNodeInfo myself,pPCConn conn){
    // fresh id and send it
    int newid = AGENT_ID_ASK();
    AGENT_SENDID_FROM_PCConn(conn,newid);
    //  get server node info
    pPCNodeInfo server = m_agentInfo_Recv(conn);
    //  add server to tree
    m_AddNeighborProxy(server);
    SYNC_SendChildInfoUpper(
        PCMANAGER_Get_RootID(),
        server->id,
        server->OSType,
        server->PCName);
    PCNODE_Free(server);
    server = NULL;
    return WHEN_IAM_WITHADMIN_CONNECT_OK;
}

#define WHEN_IAM_NORMAL_NODE_ERROR  -1
#define WHEN_IAM_NORMAL_NODE_OK      1
int When_Iam_Normal_Node(pPCNodeInfo myself,pPCConn conn){
    // get my id
    int newid = AGENT_GETID_FROM_PCConn(conn);
    // set my id 
    PCMANAGER_Set_RootID(newid);
    
    // recv server agent info 
    pPCNodeInfo server = m_agentInfo_Recv(conn);
    // add server agent 
    if(server == M_INFO_RECV_ERROR){
        return WHEN_IAM_NORMAL_NODE_ERROR;
    }
    m_AddNeighborProxy(server);
    if(IAM_ADMIN_NODE == server->NodeType
        || BE_MANAGED_NOW == server->NodeType
    ){
        PCMANAGER_SETUpperAdmin(server->id);
    }
    PCNODE_Free(server);
    server = NULL;
    return WHEN_IAM_NORMAL_NODE_OK;
}

#define WHEN_CLIENT_WITH_ADMIN_ERROR   -1
#define WHEN_CLIENT_WITH_ADMIN_OK       1
int When_Client_With_Admin(pPCNodeInfo client,pPCConn conn){
    //  set myself id
    int newid = AGENT_GETID_FROM_PCConn(conn);
    PCMANAGER_Set_RootID(newid); // set myself id
    pPCNodeInfo myself = PCMANAGER_Get_RootNode();
    //  send myself info
    if(M_INFO_SEND_ERROR == m_Info_send(conn,myself)){
        return WHEN_CLIENT_WITH_ADMIN_ERROR;
    }
    // add client to tree
    m_AddNeighborProxy(client);
    // set it upper
    PCMANAGER_SETUpperAdmin(client->id);
    ChildNodeInfoSyncTrigger();
    return WHEN_CLIENT_WITH_ADMIN_OK;
}

#define WHEN_CLIENT_NORMAL_NODE_ERROR  -1
#define WHEN_CLIENT_NORMAL_NODE_OK      1
int When_Client_Normal_Node(pPCNodeInfo client,pPCConn conn){
    pPCNodeInfo myself = PCMANAGER_Get_RootNode();
    // fresh id and send it
    int newid = AGENT_ID_ASK();
    AGENT_SENDID_FROM_PCConn(conn,newid);
    client->id = newid;
    // send info
    if(M_INFO_SEND_ERROR == m_Info_send(conn,myself)){
        return WHEN_CLIENT_NORMAL_NODE_ERROR;
    }
    // add client to tree
    m_AddNeighborProxy(client);

    if(PCMANAGER_MANAGER_STATE_TRUE == 
        PCMANAGER_Manager_State()){
        SYNC_SendChildInfoUpper(
            PCMANAGER_Get_RootID(),
            client->id,
            client->OSType,
            client->PCName);
    }

    return WHEN_CLIENT_NORMAL_NODE_OK;
}

int on_NewAgent_Connect(pAgent_proto proto,pPCConn conn){
    int result = ON_NEWAGENT_CONNECT_ERROR;
    pPCNodeInfo client = NULL;
    client = m_agentInfo_Recv(conn);
    if(client == PROTO_ANALYSISPCNODEINFO_ERROR){
        Printf_Error("info == NULL or myself == NULL");
        result = ON_NEWAGENT_CONNECT_ERROR;
    }
    else{
        switch(client->NodeType){
        case IAM_ADMIN_NODE:
        case BE_MANAGED_NOW:
            Printf_DEBUG("client is admin or with admin");
            When_Client_With_Admin(client,conn);
            break;
        case MYSELF_NODE:
            Printf_DEBUG("client is normal node");
            When_Client_Normal_Node(client,conn);
            break;
        default:
            Printf_DEBUG("UNKNOWN client node type %d\n",client->NodeType);
            break;
        }
        result = ON_NEWAGENT_CONNECT_OK;
    }
    PCNODE_Free(client);
    return result;
}
