/**
* File Name :AgentManager/AgentMsgHandle.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 16 Nov 2015 10:22:24 PM PST
*/

#include "AgentMsgHandle.h"
#include "PCNodeManager.h"
#include "AgentConversationProxy.h"

#define MAXCMD_LEN               2
#define MAX_SYNCNODE_INFO_LEN  300
#define CMD_AGENT_SYNC_UPSTREAM  1
#define CMD_ID_ASK               2
#define CMD_OTHERINFO            3
//****************************************
int on_ask_id(int sock);
int ASK_ID_UPPER();
//****************************************


int SendAgentInfo(int fatherid,int childid,int ostype,char *pcname){
    char buffer[MAX_SYNCNODE_INFO_LEN];
    API_m_itochar(fatherid,buffer,4);
    API_m_itochar(childid,&(buffer[4]),4);
    API_m_itochar(ostype,&(buffer[8]),4);
    memcpy(&(buffer[12]),pcname,MAX_PCNAME_LEN);

    pPCNodeInfo info = PCMANAGER_Get_FatherNode();
    if(info == PCMANAGER_GET_FATHERNODE_ERROR){
        return 0;
    }
    char cmdbuf[MAXCMD_LEN];
    cmdbuf[0]=CMD_AGENT_SYNC_UPSTREAM;
    int father_sock = info -> conn.cmd_socket;
    if( AGENT_CONVERSATIONPROXY_SENDUPSTREAMHEAD_ERROR == 
        AGENT_ConversationProxy_SendUpStreamHead(father_sock)){
        return 0;
    }
    if( SOCKET_SEND_ERROR == 
        API_socket_send(father_sock,buffer,MAX_SYNCNODE_INFO_LEN)){
        return 0;
    }
    return 1;
}

int on_recv_Agent_Info(int sock){
    char buffer[MAX_SYNCNODE_INFO_LEN];
    if( SOCKET_RECV_ERROR == 
    API_socket_recv(sock,buffer,MAX_SYNCNODE_INFO_LEN)){
        return 0;
    }
    int fatherid = API_m_chartoi(buffer,4);
    int childid  = API_m_chartoi(&(buffer[4]),4);
    int ostype   = API_m_chartoi(&(buffer[8]),4);
    char pcname[MAX_PCNAME_LEN];
    memcpy(pcname,&(buffer[12]),MAX_PCNAME_LEN);
    pPCNodeInfo node = PCNODE_Create();
    if(node == NULL){ return 0; }
    int res = PCNODE_SETAllData(
            node,          //node
            childid,       // id
            ostype,        // OSType
            pcname,        // pcname
            UNKONWN_NODE,  // NodeType
            -1,            // conntype
            "",            // ip
            -1,            // port
            -1);           // cmd_sock
    PCMANAGER_ADDRemote(fatherid,node);
    return 0;
}

int SendUpStream   (char *msg,int msglen){
    if(msglen > 255){
        Printf_Error("The info UpStream OverFlow");
        return 0;
    }

    pPCNodeInfo info = PCMANAGER_Get_FatherNode();
    char cmdbuf[MAXCMD_LEN];
    cmdbuf[0] = CMD_AGENT_SYNC_UPSTREAM;
    if(info == PCMANAGER_GET_FATHERNODE_ERROR){
        return 0;
    }
    int father_sock = info->conn.cmd_socket;
    if( AGENT_CONVERSATIONPROXY_SENDUPSTREAMHEAD_ERROR == 
        AGENT_ConversationProxy_SendUpStreamHead(father_sock)
        || SOCKET_SEND_ERROR == 
         API_socket_send(father_sock,cmdbuf,MAXCMD_LEN)){
        return 0;
    }
    return 1;
}

int SendDownStream (char *msg,int msglen){
    return 0;
}

int on_UpStreamMsg_Arrive   (int sock){
    char cmdbuf[MAXCMD_LEN];
    API_socket_recv(sock,cmdbuf,MAXCMD_LEN);
    switch(cmdbuf[0]){
    case CMD_AGENT_SYNC_UPSTREAM:
        Printf_OK("start sync here");
        on_recv_Agent_Info(sock);
        break;
    case CMD_ID_ASK:
        Printf_OK("ID ASK");
        on_ask_id(sock);
        break;
    }
    return 1;
}

int on_DownStreamMsg_Arrive (char *msg,int msglen);

int ASK_NEW_ID(){
    int id = -1;
    pPCNodeInfo self = PCMANAGER_Get_RootNode();
    switch(self->NodeType){
        case IAM_ADMIN_NODE:
        case MYSELF_NODE:
            id = PCMANAGER_Get_Fresh_ID();
            break;
        case WITH_ADMIN_NODE:
            id = ASK_ID_UPPER();
            break;
        default:
            Printf_Error("ASK_NEW_ID NO CMD_");
            break;
    }
    if(id == -1){
        Printf_Error("ASK_NEW_ID GETID_ERROR");
        return -1;
    }
    return id;
}

int on_ask_id(int sock){
    int id = ASK_NEW_ID();
    char idbuf[4];
    API_m_itochar(id,idbuf,4);
    API_socket_send(sock,idbuf,4);
    return 1;
}

int ASK_ID_UPPER(){
    pPCNodeInfo info = PCMANAGER_Get_FatherNode();
    char cmdbuf[MAXCMD_LEN];
    cmdbuf[0] = CMD_ID_ASK;
    if(info == PCMANAGER_GET_FATHERNODE_ERROR)
    {
        return -1;
    }
    // get father socket
    int father_sock = info->conn.cmd_socket;
    if( AGENT_CONVERSATIONPROXY_SENDUPSTREAMHEAD_ERROR == 
        AGENT_ConversationProxy_SendUpStreamHead(father_sock)
    ){
        return -1;
    }
    if( SOCKET_SEND_ERROR == 
        API_socket_send(father_sock,cmdbuf,MAXCMD_LEN)){
        return -1;
    }
    char idbuf[4];
    if( SOCKET_RECV_ERROR == 
        API_socket_recv(father_sock,idbuf,4)){
        return -1;
    }
    return API_m_chartoi(idbuf,4);
}

