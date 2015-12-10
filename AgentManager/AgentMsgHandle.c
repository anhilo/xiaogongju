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
#define CMD_RESET_MYID           4
#define CMD_OTHERINFO            5
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
Printf_DEBUG("NoFather");
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
        API_socket_send(father_sock,cmdbuf,MAXCMD_LEN)){
        return SENDUPPER_ERROR;
    }
    if( SOCKET_SEND_ERROR == 
        API_socket_send(father_sock,msg,MAX_SYNCNODE_INFO_LEN)){
        return SENDUPPER_ERROR;
    }
    return SENDUPPER_OK;
}


//==============================================================================
//   ASK_NEW_ID       &&   on_ask_id
//------------------------------------------------------------------------------
int ASK_NEW_ID(){
    int id = -1;

    MIC_USLEEP(1);
    int manage_now = PCMANAGER_Manager_Now();
    Printf_DEBUG("manager now --> %d",manage_now);
    switch(manage_now){
        case PCMANAGER_MANAGER_NOW_TRUE:
            id = ASK_ID_UPPER();
            break;
        case PCMANAGER_MANAGER_NOW_FALSE:
            id = PCMANAGER_Get_Fresh_ID();
            break;
        default:
            Printf_Error("GET FRESH ID ERROR");
            break;
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
    return SendUpper(CMD_ID_REPLACE 
        ,buffer,MAX_SYNCNODE_INFO_LEN);
}

int on_upper_replaceID(int sock){
    char buffer[MAX_SYNCNODE_INFO_LEN];
    API_socket_recv(sock,buffer,MAX_SYNCNODE_INFO_LEN);
    int oldid = API_m_chartoi(buffer,4);
    int newid = API_m_chartoi(&(buffer[4]),4);
Printf_OK("On upper replace ID msg %d -> %d .",
        oldid,newid);
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
    Printf_DEBUG("father id = %d , id = %d , ostype = %d , pcname = %s",
        fatherid,childid,ostype,pcname);
    return SendUpper(CMD_AGENT_SYNC_UPSTREAM 
        ,buffer,MAX_SYNCNODE_INFO_LEN);
}

int on_recv_Agent_Info(int sock){
    char buffer[MAX_SYNCNODE_INFO_LEN];
    int  res2 = API_socket_recv(sock,buffer, MAX_SYNCNODE_INFO_LEN);
    if( SOCKET_RECV_ERROR == res2
        ){
        return 0;
    }
Printf_DEBUG("recv = %d",res2);
    int fatherid = API_m_chartoi(buffer,4);
    int childid  = API_m_chartoi(&(buffer[4]),4);
    int ostype   = API_m_chartoi(&(buffer[8]),4);
    char pcname[MAX_PCNAME_LEN];
    memcpy(pcname,&(buffer[12]),MAX_PCNAME_LEN);
Printf_DEBUG("father id = %d , id = %d , ostype = %d , pcname = %s",
            fatherid,childid,ostype,pcname);
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
    int res3;
    if(fatherid != PCMANAGER_Get_RootID()){
        res3 = PCMANAGER_ADDRemote(
                fatherid,node);
    }
    SendAgentInfo(fatherid,childid,
            ostype,pcname);
    Printf_DEBUG("ADD Remote %d",res3);
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
        //m_analyze_and_Doit(buffer);
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
#define MAX_DOWNSTREAM_CMD_LEN      5
// offset 1 消息类型，直达消息还是广播消息
// offset 2 当为定点消息时，这里为目标ID

#define CMD_DOWN_DIRECT_POINT_MSG   1   // 直达消息
#define CMD_DOWN_BROADCAST_MSG      2   // 广播类消息
//#define CMD_AGENT_SYNC_UPSTREAM  1   //  复用向上的消息功能
//#define CMD_ID_REPLACE           2
//#define CMD_ID_ASK               3
//#define CMD_OTHERINFO            4
//****************************************
#define SENDDOWN_OK        1
#define SENDDOWN_ERROR     0

int buflock = 0;  // if(lock) wait
char cmdbuf_for_downbroadcast[MAX_DOWNSTREAM_CMD_LEN];
char msgbuf_for_downbroadcast[MAX_SYNCNODE_INFO_LEN];

int m_for_each_downStreamNode(pNodeData info){
    if(info == NULL) {return 0;}
    if(((pPCNodeInfo)info) -> NodeType == UPSTREAM_NODE){
        // is up stream node
        return 1;
    }
    // is down stream
    int nextsock = ((pPCNodeInfo)info) ->conn.cmd_socket;
    if(nextsock != -1){
        AGENT_ConversationProxy_SendDownStreamHead(nextsock);
        API_socket_send(nextsock,cmdbuf_for_downbroadcast
            ,MAX_DOWNSTREAM_CMD_LEN);
        API_socket_send(nextsock,msgbuf_for_downbroadcast
            ,MAX_SYNCNODE_INFO_LEN);
    }
    return 1;
}

int SendDownStream(int broadcasttype,int targetid
        ,char *msg,int msglen){
    pPCNodeInfo info;
    while(buflock) MIC_USLEEP(300);
    buflock = 1;
    cmdbuf_for_downbroadcast[0] = broadcasttype;
    API_m_itochar(targetid,&(cmdbuf_for_downbroadcast[1]),4);
    if(msglen > MAX_SYNCNODE_INFO_LEN) 
        return SENDUPPER_ERROR;
    memcpy(msgbuf_for_downbroadcast,msg,MAX_SYNCNODE_INFO_LEN);
    switch(cmdbuf_for_downbroadcast[0]){
        case CMD_DOWN_DIRECT_POINT_MSG :   // 直达消息:
            info = PCMANAGER_GETNextJump(targetid);
            m_for_each_downStreamNode(info);
            break;
        case CMD_DOWN_BROADCAST_MSG :  // 广播类消息
            PCMANAGER_Traversal_Neighbor(
                m_for_each_downStreamNode);
            break;
        default:
            Printf_Error("Send Down Stream Error");
            break;
    }
    buflock = 0;
    return 0;
}

int SendDown_BroadCast(char *msg,int msglen){
    return SendDownStream(CMD_DOWN_BROADCAST_MSG,
        0, msg,msglen);
}

int SendDown_DirectMsg(int targetid ,char *msg,int msglen){
    return SendDownStream(CMD_DOWN_DIRECT_POINT_MSG,
        targetid,msg,msglen);
}

int m_ID_Replace(char *msgbuf_1){
    int oldid = API_m_chartoi(&(msgbuf_1[1]),4);
    int newid = API_m_chartoi(&(msgbuf_1[5]),4);
    Printf_OK("[ Replace Msg ] %d -> %d . ",oldid,newid);
    return 1;
}

int m_analyze_and_Doit(char *msgbuf_1){
//    Printf_DEBUG("The recv MSG is %s",msgbuf_1);
    switch(msgbuf_1[0]){
    case CMD_ID_REPLACE:
        Printf_OK("Recv ID Replace msg");
        m_ID_Replace(msgbuf_1);
        break;
    case CMD_RESET_MYID:
        on_reset_myself_id(&(msgbuf_1[1]),
            MAX_SYNCNODE_INFO_LEN-1);
        break;
    default:
        Printf_Error("Error m_analyze_and_Doit");
        break;
    }
    return 1;
}

int on_recvDirectPoint_Msg(int sock,char *cmdbuf_1,
        char *msgbuf_1){
    int targetid =API_m_chartoi(&(cmdbuf_1[1]),4);
    if(targetid == PCMANAGER_Get_RootID()){
        // 解析消息类型并处理
        m_analyze_and_Doit(msgbuf_1);
    }
    else{
        // 继续向下层节点传递消息
        SendDownStream(CMD_DOWN_DIRECT_POINT_MSG,
            targetid,msgbuf_1,
            MAX_SYNCNODE_INFO_LEN);
    }
    return 1;
}

int on_broadcast_Down_msg(int sock,char *cmdbuf_1,
        char *msgbuf_1){
Printf_DEBUG("Down MSG?????????????????");
    // 解析这一消息
    m_analyze_and_Doit(msgbuf_1);
    // 继续向子节点方向传递
    SendDownStream (CMD_DOWN_BROADCAST_MSG,0,
        msgbuf_1,MAX_SYNCNODE_INFO_LEN);
    return 1;
}

int on_DownStreamMsg_Arrive (int sock){
    char cmdbuf_1[MAX_DOWNSTREAM_CMD_LEN];
    char msgbuf_1[MAX_SYNCNODE_INFO_LEN];
    int recvn1 = API_socket_recv(sock,cmdbuf_1,MAX_DOWNSTREAM_CMD_LEN);
    int recvn2 = API_socket_recv(sock,msgbuf_1,MAX_SYNCNODE_INFO_LEN);
//    Printf_Error("n1 = %d,n2 = %d",recvn1,recvn2);
//    Printf_Error("recv[1] = %d, recv[2] = %d, recv[3] = %d, recv[4] = %d",msgbuf_1[1],msgbuf_1[2],msgbuf_1[3],msgbuf_1[4]);
    switch(cmdbuf_1[0]){
        case CMD_DOWN_DIRECT_POINT_MSG:  // 直达消息
            on_recvDirectPoint_Msg(sock,cmdbuf_1,msgbuf_1);
            break;
        case CMD_DOWN_BROADCAST_MSG:     // 广播消息
            on_broadcast_Down_msg(sock,cmdbuf_1,msgbuf_1);
            break;
    }
    return 1;
}

int SendDownReplaceID(int oldid,int newid){
    char cmd[MAX_SYNCNODE_INFO_LEN];
    cmd[0] = CMD_ID_REPLACE;
    API_m_itochar(oldid,&(cmd[1]),4);
    API_m_itochar(newid,&(cmd[5]),4);
    SendDown_BroadCast(cmd,MAX_SYNCNODE_INFO_LEN);
    return 1;
}

//*********************************************
// Broadcast_ReplaceID(int oldid,int newid);
//*********************************************
int Broadcast_ReplaceID(int oldid,int newid){
    SendUpperReplaceID(oldid,newid);
    SendDownReplaceID(oldid,newid);
    return 1;
}


//*********************************************
// int resetTargetNewId(int targetid,int newid)
// int on_reset_myself_id(char *msg,int len)
//*********************************************
int resetTargetNewId(int targetid,int newid){
    char idbuf[5];
    idbuf[0] = CMD_RESET_MYID;
    API_m_itochar(newid,&idbuf[1],4);
    Printf_DEBUG("targetid = %d,newid = %d",
        targetid,newid);
    return SendDown_DirectMsg(targetid,idbuf,5);
}

int on_reset_myself_id(char *msg,int len){
    if(len < 4) {
        Printf_Error("on_reset_myself_id Error");
        return 0;
    }
    int newid = API_m_chartoi(msg,4);
    // 
    Printf_OK("my new id is ~ %d",newid);
    PCMANAGER_Set_RootID(newid);
    return 1;
}

