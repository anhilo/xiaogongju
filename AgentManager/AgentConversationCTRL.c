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
int AGENT_Conversation_Init(int ostype,char *pcname,int agent_type){
    pPCNodeInfo m_self;
    m_self= (pPCNodeInfo)PCNODE_Create();
    if(m_self == NULL){
        return AGENT_CONVERSATION_INIT_ERROR;
    }
    int res = PCNODE_SETAllData(
        m_self,     // node
        0,          // id
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
    return AGENT_CONVERSATION_INIT_OK;
}

int AGENT_Conversation_Listen(int port,int maxnum){
    return AGENT_ConversationProxy_StartServer(port,maxnum);
}

int AGENT_Conversation_Connect(char *ip,int port){
    return AGENT_ConversationProxy_Connect(ip,port);
}

int AGENT_Conversation_Build_SockTunnel(int targetid);
