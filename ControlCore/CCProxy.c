/**
* File Name :ControlCore/CCProxy.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 14 Dec 2015 02:29:38 AM PST
*/

#include "CCProxy.h"
#include "CC_AgentConn.h"
#include "CC_ShellCtrl.h"
#include "../AgentManager/AgentTunnelHandle.h"
#include "../AgentManager/AgentConversationCTRL.h"
#include "../AgentManager/PCNodeInfo.h"
#include "Cmd_Ctrl.h"
#include "CC_Msg_module.h"

//==============================================
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
        pPCConn targetsock = CMDCTRL_BuildTargetSock(targetid,
            AGENT_CONN_COMMAND_LISTEN);
        if(targetsock == CMDCTRL_BUILDTARGETSOCK_ERROR){
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
        targetsock = CMDCTRL_BuildTargetSock(targetid,
            AGENT_CONN_COMMAND_CONNECT);
        if(targetsock == CMDCTRL_BUILDTARGETSOCK_ERROR){
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

int CCProxy_SendMsg(int targetid,char *msg,int msglen){
    int res = CCMSG_SendMsg(targetid,msg,msglen);
    if(res == CCMSG_SENDMSG_ERROR ){
        return 0;
    }
    return 1;
}

int m_CCProxy_CMD_Send(pPCConn conn,int cmd){
    char cmdmsg[CCPROXY_CMD_LEN];
    cmdmsg[0] = cmd;
    int res = PCCONN_SendData(conn,cmdmsg,CCPROXY_CMD_LEN);
    if( PCCONN_SENDDATA_ERROR == res){
        return 0;
    }
    return 1;
}

int CCProxy_startShell(int targetid,int port,int maxclient){
    int res = CC_StartShell(targetid,port);
    if(res == CC_STARTSHELL_ERROR){
        return CCPROXY_STARTSHELL_ERROR;
    }
    return CCPROXY_STARTSHELL_OK;
}
