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
#define CMD_ID_REPLACE           2
#define CMD_ID_ASK               3
#define CMD_OTHERINFO            4
//****************************************
int on_ask_id(int sock);
int ASK_ID_UPPER();
//****************************************

//==============================================================================
//   SendUpper(int cmd,char *msg,int len)
//------------------------------------------------------------------------------
#define SENDUPPER_OK        1
#define SENDUPPER_ERROR     0
int SendUpper(int cmd,char *msg,int len){
    if(len > MAX_SYNCNODE_INFO_LEN) 
        return SENDUPPER_ERROR;
    pPCNodeInfo info = PCMANAGER_Get_FatherNode();
    if(info == PCMANAGER_GET_FATHERNODE_ERROR){
        return SENDUPPER_ERROR;
    }
    char cmdbuf[MAXCMD_LEN];
    cmdbuf[0]=cmd;
    int father_sock = info -> conn.cmd_socket;
    if( AGENT_CONVERSATIONPROXY_SENDUPSTREAMHEAD_ERROR == 
        AGENT_ConversationProxy_SendUpStreamHead(father_sock)){
        return SENDUPPER_ERROR;
    }
    if( SOCKET_SEND_ERROR == 
        API_socket_send(father_sock,buffer,MAX_SYNCNODE_INFO_LEN)){
        return SENDUPPER_ERROR;
    }
    return SENDUPPER_OK;
}


//==============================================================================
//   ASK_NEW_ID       &&   on_ask_id
//------------------------------------------------------------------------------
int ASK_NEW_ID(){
    int id = -1;
    pPCNodeInfo self = PCMANAGER_Get_RootNode();
    switch(self->NodeType){
        case IAM_ADMIN_NODE:
        case MYSELF_NODE:
            id = PCMANAGER_Get_Fresh_ID();
            break;
        case UPSTREAM_NODE:
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

//==============================================================================
//   SendUpperReplaceID       &&   on_upper_replaceID
//------------------------------------------------------------------------------
int SendUpperReplaceID(int oldid,int newid){
    char buffer[MAX_SYNCNODE_INFO_LEN];
    API_m_itochar(oldid,buffer,4);
    API_m_itochar(newid,&(buffer[4]),4);
    return SendUpperInfo(CMD_ID_REPLACE 
        ,buffer,MAX_SYNCNODE_INFO_LEN);
}

int on_upper_replaceID(int sock){
    char buffer[MAX_SYNCNODE_INFO_LEN];
    API_socket_recv(sock,buffer,MAX_SYNCNODE_INFO_LEN);
    int oldid = API_m_chartoi(buffer,4);
    int newid = API_m_chartoi(&(buffer[4]),4);
    SendUpperReplaceID(oldid,newid);
    return 1;
}

//==============================================================================
//   SendAgentInfo       &&   on_recv_Agent_Info
//------------------------------------------------------------------------------
int SendAgentInfo(int fatherid,int childid,int ostype,char *pcname){
    char buffer[MAX_SYNCNODE_INFO_LEN];
    API_m_itochar(fatherid,buffer,4);
    API_m_itochar(childid,&(buffer[4]),4);
    API_m_itochar(ostype,&(buffer[8]),4);
    memcpy(&(buffer[12]),pcname,MAX_PCNAME_LEN);
    return SendUpperInfo(CMD_AGENT_SYNC_UPSTREAM 
        ,buffer,MAX_SYNCNODE_INFO_LEN);
}

int on_recv_Agent_Info(int sock){
    char buffer[MAX_SYNCNODE_INFO_LEN];
    if( SOCKET_RECV_ERROR == 
        API_socket_recv(sock,buffer,
            MAX_SYNCNODE_INFO_LEN)){
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

//==============================================================================
//   on_UpStreamMsg_Arrive (int sock)
//------------------------------------------------------------------------------

int on_UpStreamMsg_Arrive   (int sock){
    char cmdbuf[MAXCMD_LEN];
    API_socket_recv(sock,cmdbuf,MAXCMD_LEN);
    switch(cmdbuf[0]){
    case CMD_AGENT_SYNC_UPSTREAM:
        Printf_OK("start sync here");
        on_recv_Agent_Info(sock);
        break;
    case CMD_ID_REPLACE:
        Printf_OK("id need replace");
        on_upper_replaceID(sock);
        break;
    case CMD_ID_ASK:
        Printf_OK("ID ASK");
        on_ask_id(sock);
        break;
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////           DownStream Start from here            //////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//****************************************
#define MAX_DOWNSTREAM_CMD_LEN      2
// offset 1 消息类型，直达消息还是广播消息
// offset 2 消息功能，修改ID还是广播节点信息

#define CMD_DOWN_DIRECT_POINT_MSG   1   // 直达消息
#define CMD_DOWN_BROADCAST_MSG      2   // 广播类消息
//#define CMD_AGENT_SYNC_UPSTREAM  1   //  复用向上的消息功能
//#define CMD_ID_REPLACE           2
//#define CMD_ID_ASK               3
//#define CMD_OTHERINFO            4
//****************************************
#define SENDDOWN_OK        1
#define SENDDOWN_ERROR     0

int buflok = 0;  // if(lock) wait
char cmdbuf[MAX_DOWNSTREAM_CMD_LEN];
char msgbuf[MAX_SYNCNODE_INFO_LEN];

int SendDownStream (int broadcasttype,int cmd,int targetid
        ,char *msg,int msglen){
    while(buflock) MIC_USLEEP(300);
    buflock = 1;
    cmdbuf[0] = broadcasttype;
    cmdbuf[1] = cmd;
    if(msglen > MAX_SYNCNODE_INFO_LEN) 
        return SENDUPPER_ERROR;
    memcpy(msgbuf,msg,MAX_SYNCNODE_INFO_LEN);
    switch(cmdbuf[0]){
      case CMD_DOWN_DIRECT_POINT_MSG:   // 直达消息
        pPCNodeInfo info = PCMANAGER_GETNextJump(targetid);
        m_for_each_downStreamNode(info);
        break;
      case CMD_DOWN_BROADCAST_MSG:  // 广播类消息
        PCMANAGER_Traversal_Neighbor(m_for_each_downStreamNode);
        break;
    }
    buflock = 0;
    return 0;
}

int m_for_each_downStreamNode(pPCNodeInfo info){
    pPCNodeInfo info = PCMANAGER_GETNextJump(targetid);
    if(info == NULL) {return 0;}
    if(info -> NodeType == UPSTREAM_NODE){
        // is up stream node
        return 1;
    }
    // is down stream
    int nextsock = info->conn.cmd_socket;
    if(nextsock != -1){
        AGENT_ConversationProxy_SendDownStreamHead(nextsock);
        API_socket_send(nextsock,cmdbuf,MAX_DOWNSTREAM_CMD_LEN);
        API_socket_send(nextsock,msgbuf,MAX_SYNCNODE_INFO_LEN);
    }
    return 1;
}

int on_recvDirectPoint_smg(int sock){
    char msgbuf_1[MAX_SYNCNODE_INFO_LEN];
    API_socket_recv(sock,msgbuf_1,MAX_SYNCNODE_INFO_LEN);
    int targetid = API_m_chartoi(&(msgbuf_1[4]),4);
    if(targetid == PCMANAGER_Get_RootID()){
        Printf_OK("Down broadcast Msg arrived");
    }
    else{   // 继续传递消息
        SendDownStream(
    }
    return 1;
}

int on_DownStreamMsg_Arrive (int sock){
    char cmdmsg_1[MAX_DOWNSTREAM_CMD_LEN];
    API_socket_recv(sock,cmdmsg_1,MAX_DOWNSTREAM_CMD_LEN);
    switch(cmdmsg[0]){
        case CMD_DOWN_DIRECT_POINT_MSG:  // 直达消息
            on_recvDirectMsg(sock);
            break;
        case CMD_DOWN_BROADCAST_MSG:     // 广播消息
//            on_broadcast_Down_msg(sock);
            break;
    }
    return 1;
}

