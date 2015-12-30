/**
* File Name :AgentManager/AgentConversationProxy.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 日 11/15 20:35:09 2015
*/
#include "AgentConversationProxy.h"
#include "PCNodeInfo.h"
#include "PCNodeManager.h"
#include "AgentConnHandle.h"
#include "AgentTunnelHandle.h"
#include "AgentMsgHandle.h"
//*******************************************************
#define AGENTCMD_MAX_LEN  1

#define CMDMSG_NEW_NODE_CONNECT    1
#define CMDMSG_NEW_TUNNEL_ASK      2
#define CMDMSG_NEW_TUNNEL_ANSWER   3
#define CMDMSG_UPPER_BROADCAST_MSG 4
#define CMDMSG_DOWN_BROADCAST_MSG  5

int StartListenerThread();
//*******************************************************


int AGENT_ConversationProxy_Init(){
    StartListenerThread();
    return AGENT_CONVERSATIONPROXY_INIT_OK;
}


int m_Listener_For_EachAgentNode(pNodeData node){
    int res = 1;
    return res;
}

MIC_THREAD_FUN_DEF(Listener_Thread,info){
    while(1){
        PCMANAGER_Traversal_Neighbor(m_Listener_For_EachAgentNode);
        MIC_USLEEP(100);
    }
    return 0;
}

int StartListenerThread(){
    MIC_THREAD_HANDLE_ID thread_id;
    if(MIC_THREAD_CREATE(thread_id,Listener_Thread,NULL)<0){
        Printf_Error("Thread id create Error");
        return 0;
    }
//    MIC_THREAD_JOIN(thread_id);
    MIC_USLEEP(10);
    return 1;
}

int m_callBackForEachAccept(int socket){
    pPCConn conn = PROTO_CreatePCConnFromSocket(socket);
    if(conn == PROTO_CREATEPCCONNFROMSOCKET_ERROR){
        return 0;
    }
    
    return 1;
}

int AGENT_ConversationProxy_StartServer(int port,int maxnum){
    /// start server and set callback funs
    int sock = API_socket_init_server(port,maxnum);
    if(sock!=SOCKET_SERVER_INIT_ERROR){
        if(API_SOCKET_SERVER_START_ERROR 
            != API_socket_server_start(sock,m_callBackForEachAccept)){
            return AGENT_CONVERSATIONPROXY_STARTSERVER_OK;
        }
    }
    return AGENT_CONVERSATIONPROXY_STARTSERVER_ERROR;
}

int AGENT_ConversationProxy_Connect(char *ip,int port){
    /// conn target and reflush id
    return AGENT_CONVERSATIONPROXY_CONNECT_OK;
}

