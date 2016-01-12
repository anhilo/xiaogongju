/**
* File Name :AgentManager/AgentConversationCTRL.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 日 11/15 20:10:07 2015
*/
#include "AgentConversationCTRL.h"
#include "AgentConversationProxy.h"
#include "PCNodeInfo.h"
#include "PCNodeManager.h"
#include "AgentTunnelHandle.h"
int AGENT_Conversation_Init(int ostype,char *pcname,int agent_type){
    pPCNodeInfo m_self;
    int mid = (agent_type == IAM_ADMIN_NODE)?0:1;
    m_self= (pPCNodeInfo)PCNODE_Create();
    if(m_self == NULL){
        return AGENT_CONVERSATION_INIT_ERROR;
    }
Printf_DEBUG("agent_type = %d",agent_type);
    int res = PCNODE_SETAllData(
        m_self,     // node
        mid,        // id
        ostype,     // ostype
        pcname,     // pcname
        agent_type, //nodetype
        -1,         //conntype
        "",         // ip
        -1,         // port
        0);         // cmd_sock
    if(res == PCNODE_SETALLDATA_ERROR){
        return AGENT_CONVERSATION_INIT_ERROR;
    }
    res = PCMANAGER_INIT(m_self);
    if(res == PCMANAGER_INIT_ERROR){
        return AGENT_CONVERSATION_INIT_ERROR;
    }

    AGENT_ConversationProxy_Init();
    return AGENT_CONVERSATION_INIT_OK;
}

int AGENT_Conversation_Listen(int port,int maxnum){
    return AGENT_ConversationProxy_StartServer(port,maxnum);
}

int AGENT_Conversation_Connect(char *ip,int port){
    return AGENT_ConversationProxy_Connect(ip,port);
}

pPCConn AGENT_Conversation_Build_SockTunnel(int targetid){
    pPCConn conn = AGENT_TUNN_BuildTunnel(targetid);
    if(AGENT_TUNN_BUILDTUNNEL_ERROR == conn){
        return AGENT_CONVERSATION_BUILD_SOCKTUNNEL_ERROR;
    }
    return conn;
}

int AGENT_Conversation_GetID(){
    return PCMANAGER_Get_RootID();
}
