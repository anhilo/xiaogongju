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
//*******************************************************
#define AGENTCMD_MAX_LEN  1

#define CMDMSG_NEW_NODE_CONNECT    1
#define CMDMSG_NEW_TUNNEL_ASK      2
#define CMDMSG_NEW_TUNNEL_ANSWER   3
#define CMDMSG_UPPER_BROADCAST_MSG 4
#define CMDMSG_DOWN_BROADCAST_MSG  5
//*******************************************************
int m_socket_send(int sock,char *buffer,int len){
    return API_socket_send(sock,buffer,len);
}

int m_socket_recv(int sock,char *buffer,int len){
    return API_socket_recv(sock,buffer,len);
}

int AGENT_ConversationProxy_Init(){
    StartListenerThread();
    return AGENT_CONVERSATIONPROXY_INIT_OK;
}


int m_CMD_Listen(int sock,char *ip,int port){
    char cmdmsg[AGENTCMD_MAX_LEN];

    int res = API_socket_read_state(sock,1,0);

    if(SOCKET_CAN_READ_STATE == res){
        int nrecv = m_socket_recv(sock,cmdmsg,AGENTCMD_MAX_LEN);
        if(nrecv != SOCKET_RECV_ERROR 
){
            switch(cmdmsg[0]){
                case CMDMSG_NEW_NODE_CONNECT:
Printf_Error("CONNECT ????");
                    break;
                case CMDMSG_NEW_TUNNEL_ASK:
Printf_DEBUG("New Tunnel ASK Here");
        on_reverse_Tunnel_Ask(sock,ip,port);
                    break;
                case CMDMSG_NEW_TUNNEL_ANSWER:
Printf_Error("NEW TUNNEL ANSWER????");
                    break;
                case CMDMSG_UPPER_BROADCAST_MSG:
Printf_DEBUG("UPPER BROADCAST MSG HERE ");
                    break;
                case CMDMSG_DOWN_BROADCAST_MSG:
Printf_DEBUG("DOWN BROADCAST MSG HERE ");
                    break;
            }
        }
        else{
            return 0;
        }
    }
    return 1;
}

int m_Listener_For_EachAgentNode(pNodeData node){
    char ip[MAX_IP_ADDR_LEN];
    int port;
    int cmd_sock = ((pPCNodeInfo)node)->conn.cmd_socket;
    memcpy(ip,((pPCNodeInfo)node)->conn.IPaddr,MAX_IP_ADDR_LEN);
    port = ((pPCNodeInfo)node)->conn.port;
    int res = m_CMD_Listen(cmd_sock,ip,port);
    if(res == 0) {
        Printf_DEBUG("Need remove this node???");
        PCMANAGER_RemoveNode(((pPCNodeInfo)node)->id);
    }
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
            on_Agent_Connect(socket);
            break;
        case CMDMSG_NEW_TUNNEL_ASK:
            Printf_OK("New Tunnel Ask");
            break;
        case CMDMSG_NEW_TUNNEL_ANSWER:
            Printf_OK("New Tunnel Answer");
on_reverse_Reply(socket);
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
    if( AGENTCONN_INTERACTIVE_OK 
        !=  AgentInfo_Interactive(sock,ip,port)){
        return AGENT_CONVERSATIONPROXY_CONNECT_ERROR;
    }
    
    return AGENT_CONVERSATIONPROXY_CONNECT_OK;
}

#define SENDCMD_HEAD_ERROR      -1
#define SENDCMD_HEAD_OK          1
int sendCMDHead(int sock,char cmd){
    char cmdmsg[AGENTCMD_MAX_LEN];
    cmdmsg[0] = cmd;
    int res = m_socket_send(sock,cmdmsg,AGENTCMD_MAX_LEN);
    Printf_DEBUG("cmdmsg[0] == %d",cmdmsg[0]);
    if(res == SOCKET_SEND_ERROR){ 
        return SENDCMD_HEAD_ERROR;
    }
    return SENDCMD_HEAD_OK;
}

// For AgentTunnelHandle
int AGENT_ConversationProxy_SendRcHead(int sock){
    if(SENDCMD_HEAD_ERROR == 
        sendCMDHead(sock,CMDMSG_NEW_TUNNEL_ASK)){ 
        return AGENT_CONVERSATIONPROXY_SENDREVERSEID_ERROR;
    }
    return AGENT_CONVERSATIONPROXY_SENDREVERSEID_OK;
}

int AGENT_ConversationProxy_Tunnel_Reply(int sock){
    if(SENDCMD_HEAD_ERROR == 
        sendCMDHead(sock,CMDMSG_NEW_TUNNEL_ANSWER)){ 
        return AGENT_CONVERSATIONPROXY_TUNNEL_REPLY_ERROR;
    }
    return AGENT_CONVERSATIONPROXY_TUNNEL_REPLY_OK;
    
}

int AGENT_ConversationProxy_Build_Tunnel(int targetid){
    return AGENT_TUNN_BuildTunnel(targetid);
//    return AGENT_CONVERSATIONPROXY_BUILD_TUNNEL_ERROR;
}

