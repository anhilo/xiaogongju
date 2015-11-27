/**
* File Name :AgentManager/AgentConnHandle.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 一 11/16 00:13:14 2015
*/
#include "AgentConnHandle.h"
#include "PCNodeManager.h"
#include "PCNodeInfo.h"
#include "AgentMsgHandle.h"


//====================================
#define M_INFO_SEND_ERROR     -1
#define M_INFO_SEND_OK         1
int m_Info_send(int sock,pPCNodeInfo info);
#define M_INFO_RECV_ERROR   NULL
pPCNodeInfo m_agentInfo_Recv(int sock);
#define M_SEND_NOTHING_ERROR  -1
#define M_SEND_NOTHING_OK      1
int m_send_nothing(int sock);
#define M_SENDID_ERROR        -1
#define M_SENDID_OK            1
int m_sendID(int sock,int id);
#define M_RECVID_ERROR        -1
int m_recvID(int sock);
//====================================

int m_AddNeighborProxy(pPCNodeInfo info){
    int res = PCMANAGER_ADDNeighbor(info);
    if(PCMANAGER_ADDNEIGHBOR_ID_CLASH == 
        res ){
// Replace ID and send Replace ID msg here
        int newid = ASK_NEW_ID();
        // send Replace ID msg here
//Broadcast_ReplaceID(info->id,newid);
        // replace id local
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


int when_IAM_ADMIN(int sock,char *ip,int port){
    pPCNodeInfo serverinfo;
    int hisid = ASK_NEW_ID();
    if(M_SENDID_ERROR != m_sendID(sock,hisid)){
        serverinfo = m_agentInfo_Recv(sock);
        if(serverinfo != M_INFO_RECV_ERROR){
            serverinfo->conn.ConnType = CONNTYPE_DIRECT_CONNECT;
            memcpy(serverinfo->conn.IPaddr,ip,MAX_IP_ADDR_LEN);
            serverinfo->conn.port = port;
            if(1 == m_AddNeighborProxy(serverinfo)){
                return 1;
            }
        }
    }
    return 0;
}

int when_IAM_NORMALNODE(int sock,char *ip,int port){
    int myid = m_recvID(sock);
    PCMANAGER_ReplaceID(PCMANAGER_Get_RootID(),myid);
    pPCNodeInfo serverinfo = m_agentInfo_Recv(sock);
    if(serverinfo->NodeType == MYSELF_NODE){
        serverinfo->conn.ConnType = CONNTYPE_DIRECT_CONNECT;
        memcpy(serverinfo->conn.IPaddr,ip,MAX_IP_ADDR_LEN);
        serverinfo->conn.port = port;
        if(1 == m_AddNeighborProxy(serverinfo)){
            return 1;
        }
    }
    return 1;
}

// be called By client
int AgentInfo_Interactive(int sock,char *ip,int port){
    pPCNodeInfo myself = PCMANAGER_Get_RootNode();
    if(myself == NULL){return AGENTCONN_INTERACTIVE_ERROR;}
    if(M_INFO_SEND_ERROR == m_Info_send(sock,myself)){
        return AGENTCONN_INTERACTIVE_ERROR;
    }
    /////// Add code From here
//    m_Info_send(sock,myself);
    switch(myself->NodeType ){
    case  MYSELF_NODE:
        when_IAM_NORMALNODE(sock,ip,port);
        break;
    case IAM_ADMIN_NODE:
        when_IAM_ADMIN(sock,ip,port);
        break;
    default:
        Printf_Error("NodeType is error");
        break;
    }
    return AGENTCONN_INTERACTIVE_OK;
}

int on_Agent_Connect(int sock){
    pPCNodeInfo clientinfo = m_agentInfo_Recv(sock);
Printf_DEBUG("the sock is -----> %d",sock);
    clientinfo -> conn.ConnType = CONNTYPE_REVERSE_CONNECT;
    if(clientinfo == NULL){return 0;}
    // add clientnode 
    pPCNodeInfo myself = PCMANAGER_Get_RootNode();
    switch(clientinfo->NodeType){
    case IAM_ADMIN_NODE:
        Printf_OK("CLIENT is Admin");
        if(0 == m_AddNeighborProxy(clientinfo)){
            Printf_Error("Add NeighborProxy Error");
            return 0;
        }
        int myid = m_recvID(sock);
        // reset my id
        PCMANAGER_ReplaceID(PCMANAGER_Get_RootID(),myid);
        // set client is upper
        PCMANAGER_SETUpperAdmin(clientinfo->id);
        // send myself
        m_Info_send(sock,myself);
        break;
    case MYSELF_NODE:
        Printf_OK("Client is Myself_node");
        int hisid = ASK_NEW_ID();
        m_sendID(sock,hisid);
        clientinfo->id = hisid;
        if(0 == m_AddNeighborProxy(clientinfo)){
            Printf_Error("Add NeighborProxy Error");
            return 0;
        }
        SendAgentInfo(PCMANAGER_Get_RootID(),clientinfo->id,
            clientinfo->OSType,clientinfo->PCName);
        m_Info_send(sock,myself);
        break;
    default:
        return 0;
    }
Printf_DEBUG("myid now is %d",PCMANAGER_Get_RootID());
    return 1;
}


//******************************************************************
#define SEND_INFO_MSG         1
#define SEND_NOTHING_MSG      2
#define RECVOK    1
#define RECVERROR 2

#define SEND_INFO_ERROR  -1
#define SEND_INFO_OK      1
int send_info(int sock,char *buffer,int len){
    int result,nrecv;
    char msg  = SEND_INFO_MSG;
    int nsend = API_socket_send(sock,&msg,1);
    if(nsend == SOCKET_SEND_ERROR){
        result =  SEND_INFO_ERROR;
    }
    else{
        nsend = API_socket_send(sock,buffer,len);
        if(nsend == SOCKET_SEND_ERROR){
            result =  SEND_INFO_ERROR;
        }
        else{
            nrecv = API_socket_recv(sock,&msg,1);
            if(nrecv == SOCKET_RECV_ERROR 
                || msg != RECVOK){
                result =  SEND_INFO_ERROR;
            }
            else{
                result = SEND_INFO_OK;
            }
        }
    }
    return result;
}

int m_send_nothing(int sock){
    int result,nsend,nrecv;
    char msg = SEND_NOTHING_MSG;
    nsend= API_socket_send(sock,&msg,1);
    if(nsend == SOCKET_SEND_ERROR){
        result = SEND_INFO_ERROR;
    }
    else{
        nrecv = API_socket_recv(sock,&msg,1);
        if(nrecv == SOCKET_RECV_ERROR
            || msg != RECVOK){
            result = SEND_INFO_ERROR;
        }
        else{
            result = SEND_INFO_OK;
        }
    }
    return result;
}

#define RECV_INFO_ERROR    -1
#define RECV_INFO_OK        1
int recv_info(int sock,char *buffer,int len){
    int result , nsend,nrecv;
    char msg ,*buf;
    nrecv  =API_socket_recv(sock,&msg,1);
    if(nrecv == SOCKET_RECV_ERROR
        || msg == SEND_NOTHING_MSG){
        result = RECV_INFO_ERROR;
    }
    else{
        buf = (char *)malloc(sizeof(char)*len+10);
        nrecv = API_socket_recv(sock,buf,len);
        if(nrecv == SOCKET_RECV_ERROR){
            result = RECV_INFO_ERROR;
        }
        else{
            msg = RECVOK;
            API_socket_send(sock,&msg,1);
            result = RECV_INFO_OK;
        }
        memcpy(buffer,buf,len);
        free(buf);
    }
    buf = NULL;
    return result;
}

int m_sendID(int sock,int id){
    char buffer[20];
    API_m_itochar(id,buffer,4);
    if(send_info(sock,buffer,4)
        ==SEND_INFO_ERROR){
        return M_SENDID_ERROR;
    }
    return M_SENDID_OK;
}

int m_recvID(int sock){
    char buffer[20];
    if(recv_info(sock,buffer,4) 
        == RECV_INFO_ERROR){
        return M_RECVID_ERROR;
    }
    return API_m_chartoi(buffer,4);
}

int m_Info_send(int sock,pPCNodeInfo info){
    char buffer[1000];
    char idbuf[4],ostype[4],nodetype[4];
    int result = M_INFO_SEND_OK;

    if(info == NULL){
        result = M_INFO_SEND_ERROR;
    }
    else{
        API_m_itochar (info->id       ,idbuf   ,4);
        API_m_itochar (info->OSType   ,ostype  ,4);
        API_m_itochar (info->NodeType  ,nodetype,4);
        strncpy(buffer, info->PCName , MAX_PCNAME_LEN);
        // send id
        if( SEND_INFO_ERROR == send_info(sock ,idbuf,4)
            // send pcname
            || SEND_INFO_ERROR == send_info(sock,buffer,MAX_PCNAME_LEN)
            // send ostype
            || SEND_INFO_ERROR == send_info(sock,ostype,  4)
            // send nodetype
            || SEND_INFO_ERROR == send_info(sock,nodetype,4)){
            result =  M_INFO_SEND_ERROR;
        }
    }
    // send error
    if(result == M_INFO_SEND_ERROR){
        Printf_Error("Send info Error");
    }
    return  result;
}

#define BUF_ERROR   -1
#define BUF_OK       1
pPCNodeInfo m_agentInfo_Recv(int sock){
    pPCNodeInfo info = PCNODE_Create();
    char pcname[MAX_PCNAME_LEN+1];
    char id[4],ostype[4],linktype[4];
    int  mid,mostype,mlinktype;
    int result = 1;
    if(info == NULL){
        result =  BUF_ERROR;
    }
    else{
        // recv id
        if( RECV_INFO_ERROR == recv_info(sock,id,4) 
            // recv name
            || RECV_INFO_ERROR == recv_info(sock,pcname,MAX_PCNAME_LEN)
            // recv ostype
            || RECV_INFO_ERROR == recv_info(sock,ostype,4)
            // recv nodetype
            || RECV_INFO_ERROR == recv_info(sock,linktype,4)){
            result = BUF_ERROR;
        }
        else{
            mid       = API_m_chartoi(id,4);
            mostype   = API_m_chartoi(ostype,4);
            mlinktype = API_m_chartoi(linktype,4);
            int res = PCNODE_SETAllData(
                info,       // node
                mid,        // id
                mostype,    // OSType
                pcname,     // pcname
                mlinktype,  // NodeType
                -1,         // conntype
                "",         // ip
                -1,         // port
                sock);      // cmd_sock
            if(res == PCNODE_SETALLDATA_ERROR){
                result =  BUF_ERROR;
            }
        }
    }

    if(result == BUF_ERROR){
        return M_INFO_RECV_ERROR;
    }
Printf_OK("id = %d,ostype = %d , nodetype = %d, pcname = %s",
        info-> id,info->OSType,info->NodeType,info->PCName);
    return info;
}

//******************************************************************
