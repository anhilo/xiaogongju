/**
* File Name :ControlCore/CCProxy.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 14 Dec 2015 02:29:38 AM PST
*/

#include "CCProxy.h"
#include "CC_AgentConn.h"
#include "../AgentManager/AgentTunnelHandle.h"
#include "../AgentManager/AgentConversationCTRL.h"
#include "../AgentManager/PCNodeInfo.h"

//==============================================
int m_CCProxy_CMD_Send(pPCConn conn,int cmd);
//==============================================

int CCProxy_Init(int ostype,char *pcname,int node_type){
    int agent_type ,os_type;
    if(node_type == NODETYPE_ADMIN_NODE){
        agent_type = IAM_ADMIN_NODE;
    }
    else{
        agent_type = MYSELF_NODE;
    }
    switch(ostype){
    case OSTYPE_LINUX_OS:
        os_type = PC_OS_LINUX;
        break;
    case OSTYPE_ARMLINUX_OS:
        os_type = PC_OS_ARMLINUX;
        break;
    case OSTYPE_WINDOW_OS:
        os_type = PC_OS_WINDOW;
        break;
    case OSTYPE_MACOSX_OS:
        os_type = PC_OS_MACOSX;
        break;
    default:
        os_type = PC_OS_UNKNOW;
        break;
    }
    int res = AGENT_Conversation_Init(os_type,pcname,agent_type);
    if(AGENT_CONVERSATION_INIT_ERROR == res){
        return CCPROXY_INIT_ERROR;
    }
    return CCPROXY_INIT_OK;
}

// 指令协议
// connect agent
#define AGENT_CONN_COMMAND_LISTEN        1
#define AGENT_CONN_COMMAND_CONNECT       2
// lcx
#define AGENT_SERVER_COMMAND_LCXTRAN     3
#define AGENT_SERVER_COMMAND_REVLCXTRAN  4
// file 
#define AGENT_SERVER_COMMAND_UPFILE      5
#define AGENT_SERVER_COMMAND_DOWNFILE    6
// socks
#define AGENT_SERVER_COMMAND_SOCKS       7
#define AGENT_SERVER_COMMAND_RVSOCKS     8
// shell
#define AGENT_SERVER_COMMAND_CMDSHELL    9
// send msg
#define AGENT_SERVER_COMMAND_SENDMSG    10
// +-------------------+-------------------+
// | CCTRL_TYPE(1byte) | CCTRL_ARGS(1byte) |
// +-------------------+-------------------+
// | LCX? SOCKS?       | args              |
// +-------------------+-------------------+

#define MAX_CCPROXY_LEN       200
#define CCTRL_TYPE_OFFSET     0

#define M_BUILDTARGETSOCK_ERROR   NULL
pPCConn m_BuildTargetSock(int targetid,int ccproxy_cmd){
    pPCConn targetsock = 
    AGENT_Conversation_Build_SockTunnel(targetid);
    if(targetsock == 
    AGENT_CONVERSATION_BUILD_SOCKTUNNEL_ERROR){
        return M_BUILDTARGETSOCK_ERROR;
    }
    if( 0 == m_CCProxy_CMD_Send(targetsock,
            ccproxy_cmd)
    ){
        return M_BUILDTARGETSOCK_ERROR;
    }
    return targetsock;
}

int CCProxy_ListenAgent(int targetid,
    int rport,int maxnum){
    if(AGENT_Conversation_GetID() == 
        targetid){
        int res = AGENT_Conversation_Listen(rport,maxnum);
        if(AGENT_CONVERSATION_LISTEN_ERROR == res){
            return CCPROXY_LISTENAGENT_ERROR;
        }
        return CCPROXY_LISTENAGENT_OK;
    }
    else{
        pPCConn targetsock = m_BuildTargetSock(targetid,
            AGENT_CONN_COMMAND_LISTEN);
        if(targetsock == M_BUILDTARGETSOCK_ERROR){
            return CCPROXY_LISTENAGENT_ERROR;
        }
        char rport_buf[4];
        char maxnum_buf[4];
        API_m_itochar(rport ,rport_buf ,4);
        API_m_itochar(maxnum,maxnum_buf,4);
        if( PCCONN_SENDDATA_ERROR == 
                PCCONN_SendData(targetsock,rport_buf ,4)
            || PCCONN_SENDDATA_ERROR == 
                PCCONN_SendData(targetsock,maxnum_buf ,4)){
            return CCPROXY_LISTENAGENT_ERROR;
        }
    }
    return CCPROXY_LISTENAGENT_OK;
}

int m_onListenAgent(pPCConn clientsock){
    char rport_buf[4];
    char maxnum_buf[4];
    if( PCCONN_RECVDATA_ERROR == 
        PCCONN_RecvData(clientsock,rport_buf,4)
      || PCCONN_RECVDATA_ERROR == 
        PCCONN_RecvData(clientsock,maxnum_buf,4)
    ){
        return 0;
    }
    int rport  = API_m_chartoi(rport_buf,4);
    int maxnum = API_m_chartoi(maxnum_buf,4);
    if( CCPROXY_LISTENAGENT_OK == 
        CCProxy_ListenAgent(
                AGENT_Conversation_GetID(),
                rport,
                maxnum
        )
    ){
        return 0;
    }
    return 1;
}

int CCProxy_AgentConnect(int targetid,
    char *remoteip,int rport){
    pPCConn targetsock = NULL;
    if(AGENT_Conversation_GetID() == 
        targetid){
        int res = AGENT_Conversation_Connect(remoteip,rport);
        if(AGENT_CONVERSATION_CONNECT_ERROR == res){
            return CCPROXY_CONNECTAGENT_ERROR;
        }
        return CCPROXY_CONNECTAGENT_OK;
    }
    else{
        targetsock = m_BuildTargetSock(targetid,
            AGENT_CONN_COMMAND_CONNECT);
        if(targetsock == M_BUILDTARGETSOCK_ERROR){
            return CCPROXY_CONNECTAGENT_ERROR;
        }
        char ipbuf[MAX_IP_ADDR_LEN];
        char portbuf[4];
        strncpy(ipbuf,remoteip,MAX_IP_ADDR_LEN);
        API_m_itochar(rport,portbuf,4);
        if( PCCONN_SENDDATA_ERROR == 
                PCCONN_SendData(targetsock,ipbuf,MAX_IP_ADDR_LEN)
            || PCCONN_SENDDATA_ERROR == 
                PCCONN_SendData(targetsock,portbuf,4)
        ){
            return CCPROXY_CONNECTAGENT_ERROR;
        }
    }
    return CCPROXY_CONNECTAGENT_OK;
}

int m_onAgentConnect(pPCConn clientsock){
    char ipbuf[MAX_IP_ADDR_LEN];
    char portbuf[4];
    if( PCCONN_RECVDATA_ERROR == 
        PCCONN_RecvData(clientsock,ipbuf,MAX_IP_ADDR_LEN)
      || PCCONN_RECVDATA_ERROR == 
        PCCONN_RecvData(clientsock,portbuf,4)
    ){
        return 0;
    }
    int targetid = AGENT_Conversation_GetID();
    int rport = API_m_chartoi(portbuf,4);
    if( CCPROXY_CONNECTAGENT_ERROR == 
        CCProxy_AgentConnect(targetid,ipbuf,rport)
    ){
        return 0;
    }
    return 1;
}

#define MAX_MSG_LEN   300
int CCProxy_SendMsg(int targetid,char *msg,int msglen){
    if(msglen > MAX_MSG_LEN || strlen(msg)>msglen){
        return 0;
    }
    pPCConn conn = m_BuildTargetSock(targetid,
        AGENT_SERVER_COMMAND_SENDMSG);
    if( M_BUILDTARGETSOCK_ERROR == conn ){
        Printf_DEBUG("SendMSG Build target tunnel Error targetid = %d",targetid);
        return 0;
    }
    PCCONN_SendData(conn,msg,msglen);
    return 1;
}

int m_onRecvMsg(pPCConn conn){
    char buffer[MAX_MSG_LEN];
    int  res = PCCONN_RecvData(conn,buffer,MAX_MSG_LEN);
    if(res > MAX_MSG_LEN){
        Printf_Error( "RECV MSG ERROR");
        return CCPROXY_SENDMSG_ERROR;
    }
    else{
        buffer[res]= '\0';
        Printf_OK("CCProxy_onNewTunnel -->%s",buffer);
    }
    return CCPROXY_SENDMSG_OK;
}


#define CCPROXY_CMD_LEN   1
int m_CCProxy_CMD_Send(pPCConn conn,int cmd){
    char cmdmsg[CCPROXY_CMD_LEN];
    cmdmsg[0] = cmd;
    int res = PCCONN_SendData(conn,cmdmsg,CCPROXY_CMD_LEN);
    if( PCCONN_SENDDATA_ERROR == res){
        return 0;
    }
    return 1;
}

int CCProxy_onNewTunnel(pPCConn conn){
    char cmdmsg[CCPROXY_CMD_LEN];
    PCCONN_RecvData(conn,cmdmsg,CCPROXY_CMD_LEN);
    switch(cmdmsg[0]){
    case AGENT_SERVER_COMMAND_SENDMSG:
        m_onRecvMsg(conn);
        break;
    case AGENT_CONN_COMMAND_LISTEN:
        Printf_DEBUG("AGENT_CONN_COMMAND_LISTEN");
        m_onListenAgent(conn);
        break;
    case AGENT_CONN_COMMAND_CONNECT:
        Printf_DEBUG("AGENT_CONN_COMMAND_CONNECT");
        m_onAgentConnect(conn);
        break;
    default :
        Printf_Error("CCProxy_onNewTunnel Error CMD(%d)",
            cmdmsg[0]);
        break;
    }
    return 1;
}
