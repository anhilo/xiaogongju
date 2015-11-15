/**
* File Name :AgentManager/AgentConversationProxy.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 日 11/15 20:35:09 2015
*/
#include "AgentConversationProxy.h"
#include "PCNodeInfo.h"
#include "PCNodeManager.h"
//*******************************************************
#define AGENTCMD_MAX_LEN  1
//*******************************************************
int m_socket_send(int sock,char *buffer,int len){
    return API_socket_send(sock,buffer,len);
}

int m_socket_recv(int sock,char *buffer,int len){
    return API_socket_recv(sock,buffer,len);
}

int AGENT_ConversationProxy_Init(){
    return AGENT_CONVERSATIONPROXY_INIT_OK;
}

#define CMDMSG_NEW_NODE_CONNECT    1
#define CMDMSG_NEW_TUNNEL_ASK      2
#define CMDMSG_NEW_TUNNEL_ANSWER   3
#define CMDMSG_UPPER_BROADCAST_MSG 4
#define CMDMSG_DOWN_BROADCAST_MSG  5

int m_callBackForEachAccept(int socket){
    if(socket == SOCKET_INIT_ERROR){
        return 0;
    }
    char cmdmsg[AGENTCMD_MAX_LEN];
    int nrecv = m_socket_recv(socket,cmdmsg,AGENTCMD_MAX_LEN);
    if(nrecv == SOCKET_RECV_ERROR){
        return 0;
    }
    switch(cmdmsg[0]){
        case CMDMSG_NEW_NODE_CONNECT:
            Printf_OK("New Node Here");
            break;
        case CMDMSG_NEW_TUNNEL_ASK:
            Printf_OK("New Tunnel Ask");
            break;
        case CMDMSG_NEW_TUNNEL_ANSWER:
            Printf_OK("New Tunnel Answer");
            break;
        case CMDMSG_UPPER_BROADCAST_MSG:
            Printf_OK("Upper Stream MSG");  
            break;
        case CMDMSG_DOWN_BROADCAST_MSG:
            Printf_OK("DOWN Stream MSG");
            break;
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
    char cmdmsg[AGENTCMD_MAX_LEN];
    cmdmsg[0]=CMDMSG_NEW_NODE_CONNECT;
    int sock = API_socket_connect(ip,port);
    if(sock == SOCKET_CONNECT_ERROR){
        return AGENT_CONVERSATIONPROXY_CONNECT_ERROR;
    }
    int res = m_socket_send(sock,cmdmsg,AGENTCMD_MAX_LEN);
    if(res == SOCKET_SEND_ERROR){
        return AGENT_CONVERSATIONPROXY_CONNECT_ERROR;
    }
    return AGENT_CONVERSATIONPROXY_CONNECT_OK;
}

int AGENT_ConversationProxy_Build_Tunnel(int targetid){
    return AGENT_CONVERSATIONPROXY_BUILD_TUNNEL_ERROR;
}
