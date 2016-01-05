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


//int when_IAM_ADMIN(int sock,char *ip,int port){
//    pPCNodeInfo serverinfo;
//    int hisid = AGENT_ID_ASK();
//    if(M_SENDID_ERROR != m_sendID(sock,hisid)){
//        serverinfo = m_agentInfo_Recv(sock);
//        if(serverinfo != M_INFO_RECV_ERROR){
//            serverinfo->conn.ConnType = CONNTYPE_DIRECT_CONNECT;
//            memcpy(serverinfo->conn.IPaddr,ip,MAX_IP_ADDR_LEN);
//            serverinfo->conn.port = port;
//            if(1 == m_AddNeighborProxy(serverinfo)){
//                return 1;
//            }
//        }
//    }
//    return 0;
//}
//
//int when_IAM_NORMALNODE(int sock,char *ip,int port){
//    int myid = m_recvID(sock);
////    PCMANAGER_ReplaceID(PCMANAGER_Get_RootID(),myid);
//    PCMANAGER_Set_RootID(myid);
//    pPCNodeInfo serverinfo = m_agentInfo_Recv(sock);
//    if(serverinfo->NodeType == MYSELF_NODE){
//        serverinfo->conn.ConnType = CONNTYPE_DIRECT_CONNECT;
//        memcpy(serverinfo->conn.IPaddr,ip,MAX_IP_ADDR_LEN);
//        serverinfo->conn.port = port;
//        if(1 == m_AddNeighborProxy(serverinfo)){
//            return 1;
//        }
//    }
//    return 0;
//}

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
//    if(M_INFO_SEND_ERROR == m_Info_send(conn,myself)){
//        return AGENTCONN_INTERACTIVE_ERROR;
//    }
    /////// Add code From here
    Printf_DEBUG("111111111111");
    m_Info_send(conn,myself);
    switch(myself->NodeType ){
    case  MYSELF_NODE:
        Printf_DEBUG("222222222222");
        When_Iam_Normal_Node(myself,conn);
//        when_IAM_NORMALNODE(sock,ip,port);
        break;
    case IAM_ADMIN_NODE:
    case BE_MANAGED_NOW:
        Printf_DEBUG("333333333333");
        When_Iam_WithAdmin_Connect(myself,conn);
        break;
    default:
        Printf_Error("NodeType is error");
        break;
    }
    return AGENTCONN_INTERACTIVE_OK;
}

int on_Agent_Connect(int sock){
//    pPCNodeInfo clientinfo = m_agentInfo_Recv(sock);
//Printf_DEBUG("the sock is -----> %d",sock);
//    clientinfo -> conn.ConnType = CONNTYPE_REVERSE_CONNECT;
//    if(clientinfo == NULL){return 0;}
//    // add clientnode 
//    pPCNodeInfo myself = PCMANAGER_Get_RootNode();
//    switch(clientinfo->NodeType){
//    case IAM_ADMIN_NODE:
//        Printf_OK("CLIENT is Admin");
//        if(0 == m_AddNeighborProxy(clientinfo)){
//            Printf_Error("Add NeighborProxy Error");
//            return 0;
//        }
//        int myid = m_recvID(sock);
//        // reset my id
//        PCMANAGER_Set_RootID(myid);
//   //     PCMANAGER_ReplaceID(PCMANAGER_Get_RootID(),myid);
//        // set client is upper
//        PCMANAGER_SETUpperAdmin(clientinfo->id);
//        // send myself
//        m_Info_send(sock,myself);
//        //ChildNodeInfoSyncTrigger();
//        break;
//    case MYSELF_NODE:
//        Printf_OK("Client is Myself_node");
//        int hisid = AGENT_ID_ASK();
//        m_sendID(sock,hisid);
//        Printf_OK("Client id is %d",hisid);
//        clientinfo->id = hisid;
//        if(0 == m_AddNeighborProxy(clientinfo)){
//            Printf_Error("Add NeighborProxy Error");
//            return 0;
//        }
//        Printf_DEBUG("Send Agent Info Here11111111111");
////        SendAgentInfo(PCMANAGER_Get_RootID(),clientinfo->id,
////            clientinfo->OSType,clientinfo->PCName);
////        m_Info_send(sock,myself);
//        break;
//    default:
//        return 0;
//    }
Printf_DEBUG("myid now is %d",PCMANAGER_Get_RootID());
    return 1;
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
    Printf_DEBUG("---> sendinfo - %s",info->PCName);
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
Printf_DEBUG("Get Server Info from here");
    pPCNodeInfo server = m_agentInfo_Recv(conn);
    //  add server to tree
    m_AddNeighborProxy(server);
    PCNODE_Free(server);
    server = NULL;
    return WHEN_IAM_WITHADMIN_CONNECT_OK;
}

#define WHEN_IAM_NORMAL_NODE_ERROR  -1
#define WHEN_IAM_NORMAL_NODE_OK      1
int When_Iam_Normal_Node(pPCNodeInfo myself,pPCConn conn){
    // get my id
    int newid = AGENT_GETID_FROM_PCConn(conn);
    Printf_DEBUG("my new id is %d",newid);
    // set my id 
    PCMANAGER_Set_RootID(newid);
    
    // recv server agent info 
    pPCNodeInfo server = m_agentInfo_Recv(conn);
    // add server agent 
    m_AddNeighborProxy(server);
    PCNODE_Free(server);
    server = NULL;
    return WHEN_IAM_NORMAL_NODE_OK;
}

#define WHEN_CLIENT_WITH_ADMIN_ERROR   -1
#define WHEN_CLIENT_WITH_ADMIN_OK       1
int When_Client_With_Admin(pPCNodeInfo client,pPCConn conn){
    pPCNodeInfo myself = PCMANAGER_Get_RootNode();
    //  set myself id
    int newid = AGENT_GETID_FROM_PCConn(conn);
    Printf_DEBUG("my new id is %d",newid);
    PCMANAGER_Set_RootID(newid); // set myself id
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
