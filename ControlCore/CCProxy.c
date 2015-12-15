/**
* File Name :ControlCore/CCProxy.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 14 Dec 2015 02:29:38 AM PST
*/

#include "CCProxy.h"
#include "../AgentManager/AgentConversationCTRL.h"
#include "../AgentManager/PCNodeInfo.h"

int CCProxy_Init(int ostype,char *pcname,int node_type){
    int agent_type ,os_type;
    if(node_type == NODETYPE_ADMIN_NODE){
        agent_type == IAM_ADMIN_NODE;
    }
    else{
        agent_type == MYSELF_NODE;
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

#define MAX_CCPROXY_LEN       2
#define CCTRL_TYPE_OFFSET     0

int m_BuildTargetSock(int targetid,int ccproxy_cmd){
    int targetsock = 
    AGENT_Conversation_Build_SockTunnel(targetid);
    if(targetsock != 
    AGENT_CONVERSATIONPROXY_BUILD_TUNNEL_ERROR){
        return CCPROXY_LISTENAGENT_ERROR;
    }
    char cmdmsg[MAX_CCPROXY_LEN];
    cmdmsg[CCTRL_TYPE_OFFSET] = 
        ccproxy_cmd;
    if(SOCKET_SEND_ERROR == 
        API_socket_send(targetsock,cmdmsg,
            MAX_CCPROXY_LEN)
    ){
        return -1;
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
        int targetsock = m_BuildTargetSock(targetid,
            AGENT_CONN_COMMAND_LISTEN);
        if(targetsock == -1){
            return CCPROXY_LISTENAGENT_ERROR;
        }
        char rport_buf[4];
        char maxnum_buf[4];
        API_m_itochar(rport ,rport_buf ,4);
        API_m_itochar(maxnum,maxnum_buf,4);
        if( SOCKET_SEND_ERROR == 
                API_socket_send(targetsock, rport_buf,4)
            || SOCKET_SEND_ERROR == 
                API_socket_send(targetsock,maxnum_buf,4)
        ){
            return CCPROXY_LISTENAGENT_ERROR;
        }
    }
    return CCPROXY_LISTENAGENT_OK;
}

int m_onListenAgent(int clientsock){
    char rport_buf[4];
    char maxnum_buf[4];
    if(SOCKET_RECV_ERROR == 
        API_socket_recv(clientsock,rport_buf,4)
      || SOCKET_RECV_ERROR == 
        API_socket_recv(clientsock,maxnum_buf,4)
    ){
        return 0;
    }
    int rport  = API_m_chartoi(rport_buf,4);
    int maxnum = API_m_chartoi(maxnum_buf,4);
    if( CCPROXY_LISTENAGENT_OK == 
    CCProxy_ListenAgent(AGENT_Conversation_GetID(),
        rport,maxnum)){
        return 0;
    }
    return 1;
}

int CCProxy_AgentConnect(int targetid,
    char *remoteip,int rport){
    if(AGENT_Conversation_GetID() == 
        targetid){
        int res = AGENT_Conversation_Connect(remoteip,rport);
        if(AGENT_CONVERSATION_CONNECT_ERROR == res){
            return CCPROXY_LISTENAGENT_ERROR;
        }
        return CCPROXY_LISTENAGENT_OK;
    }
    else{
        int targetsock = m_BuildTargetSock(targetid,
            AGENT_CONN_COMMAND_CONNECT);
        if(targetsock == -1){
            return CCPROXY_CONNECTAGENT_ERROR;
        }
        char ipbuf[MAX_IP_ADDR_LEN];
        char portbuf[4];
        strncpy(ipbuf,remoteip,MAX_IP_ADDR_LEN);
        API_m_itochar(rport,portbuf,4);
        if( SOCKET_SEND_ERROR == 
            API_socket_send(targetsock,ipbuf,MAX_IP_ADDR_LEN)
            || SOCKET_SEND_ERROR == 
            API_socket_send(targetsock,portbuf,4)
        ){
            return CCPROXY_CONNECTAGENT_ERROR;
        }
    }
    return targetsock;
}

int m_onAgentConnect(int clientsock){
    char ipbuf[MAX_IP_ADDR_LEN];
    char portbuf[4];
    API_socket_recv(clientsock,ipbuf  ,MAX_IP_ADDR_LEN);
    API_socket_recv(clientsock,portbuf,4);
    int targetid = AGENT_Conversation_GetID();
    int rport = API_m_chartoi(portbuf,4);
    if( CCPROXY_CONNECTAGENT_ERROR == 
        CCProxy_AgentConnect(targetid,ipbuf,rport)){
        return 0;
    }
    return 1;
}

int CCProxy_SendMsg(int targetsock,char *msg,int msglen){
    
}

int CCProxy_onNewTunnel(int clientsock){
    char cmdmsg[MAX_CCPROXY_LEN];
    API_socket_recv(clientsock,cmdmsg,MAX_CCPROXY_LEN);
    switch(cmdmsg[CCTRL_TYPE_OFFSET]){
    case AGENT_CONN_COMMAND_LISTEN:
        return m_onListenAgent(clientsock);
        break;
    case AGENT_CONN_COMMAND_CONNECT:
        return m_onAgentConnect(clientsock);
        break;
    default:
        return 0;
        break;
    }
    return 1;
}
