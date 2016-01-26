/**
* File Name :ControlCore/Cmd_Ctrl.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Mon 14 Dec 2015 02:24:44 AM PST
*/

#include"Cmd_Ctrl.h"
#include"CCProxy.h"
#include"CC_Msg_module.h"
#include"CCProxy.h"
#include "../AgentManager/AgentConversationCTRL.h"
#include "../AgentManager/PCConn.h"
#include "CC_ShellCtrl.h"
#include "CC_LcxCtrl.h"
#include "CC_File_Ctrl.h"
#include "CC_Agent_Socks_module.h"


pPCConn CMDCTRL_BuildTargetSock(int targetid,int ccproxy_cmd){
    pPCConn targetsock = 
    AGENT_Conversation_Build_SockTunnel(targetid);
    if(targetsock == 
    AGENT_CONVERSATION_BUILD_SOCKTUNNEL_ERROR){
        return CMDCTRL_BUILDTARGETSOCK_ERROR;
    }
    if( 0 == m_CCProxy_CMD_Send(targetsock,
            ccproxy_cmd)
    ){
        return CMDCTRL_BUILDTARGETSOCK_ERROR;
    }
    return targetsock;
}

int CMDCTRL_onNewTunnel(pPCConn conn){
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
    case AGENT_SERVER_COMMAND_SOCKS:
        Printf_DEBUG("AGENT_SERVER_COMMAND_SOCKS");
        on_CC_startSocks(conn);
        break;
    case AGENT_SERVER_COMMAND_CMDSHELL:
        CC_onStartShellhere(conn);
        break;
    case AGENT_SERVER_COMMAND_LCXTRAN:
        CC_onLcxTran(conn);
        break;
    case AGENT_SERVER_COMMAND_UPFILE:
        CC_onFile_upload(conn);
        break;
    default :
        Printf_Error("CCProxy_onNewTunnel Error CMD(%d)",
            cmdmsg[0]);
        break;
    }
    return 1;
}
