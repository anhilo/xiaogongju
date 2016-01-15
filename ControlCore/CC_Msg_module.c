/**
* File Name :ControlCore/CC_Msg_module.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : Fri 15 Jan 2016 02:04:44 AM PST
*/
#include "CC_Msg_module.h"
#include "CCProxy.h"
#include "Cmd_Ctrl.h"
#include "../AgentManager/PCConn.h"

#define MAX_MSG_LEN   300
int CCMSG_SendMsg(int targetid,char *msg,int msglen){
    if(msglen > MAX_MSG_LEN || strlen(msg)>msglen){
        return CCMSG_SENDMSG_ERROR;
    }
    pPCConn conn = CMDCTRL_BuildTargetSock(targetid,
        AGENT_SERVER_COMMAND_SENDMSG);
    if( CMDCTRL_BUILDTARGETSOCK_ERROR == conn ){
        Printf_DEBUG("SendMSG Build target tunnel Error targetid = %d",targetid);
        return CCMSG_SENDMSG_ERROR;
    }
    PCCONN_SendData(conn,msg,msglen);
    return CCMSG_SENDMSG_OK;
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

