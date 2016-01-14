/**
* File Name :AgentManager/AgentConversationProxy.c
* Author: rootkiter
* E-mail: rootkiter@rootkiter.com
* Created Time : 日 11/15 20:35:09 2015
*/
#include "AgentConversationProxy.h"
#include "PCNodeInfo.h"
#include "PCNodeManager.h"
#include "AgentJobMsg.h"
#include "AgentCMDParse.h"
#include "AgentConnHandle.h"
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
    pJobList joblist = JOB_CreateList();
    GLOBAL_SetJobList(joblist);
    return AGENT_CONVERSATIONPROXY_INIT_OK;
}

typedef struct proconn{
    pAgent_proto proto;
    pPCConn      conn;
}PROCONN,*pPROCONN;


MIC_THREAD_FUN_DEF(m_parse_do_Thread,protoconn){
    if(protoconn == NULL) return 0;
    pAgent_proto proto = 
        ((struct proconn*)protoconn)->proto;
    pPCConn conn = 
        ((struct proconn*)protoconn)->conn;
    if(CMDPARSE_AND_DO_ERROR == 
        CMDParse_And_Do(proto,conn)){
        return 0;
    }
    PCCONN_Free(conn);
    PROTO_FreeProto(proto);
    return 0;
}


int m_Listener_For_EachAgentNode(pNodeData node){
    int res = 1;
    pPCNodeInfo info = (pPCNodeInfo)node;
    pPCConn conn = PCCONN_Copy(&(info -> conn));
    pAgent_proto proto = NULL;
    if(conn == NULL || info == NULL){
        res = 0;
    }
    // socket can read
    if( PROTO_RECVSTATE_CANRECV == 
        PROTO_RecvState(conn)
    ){
        proto = PROTO_RecvProto(conn);
        if(proto == PROTO_RECVPROTO_ERROR){
            res = 0;
        }
        pPROCONN protoconn = 
            (pPROCONN)malloc(sizeof(PROCONN));
        protoconn -> proto = proto;
        protoconn -> conn  = conn;
        MIC_THREAD_HANDLE_ID thread_id;
        if(MIC_THREAD_CREATE(thread_id,
            m_parse_do_Thread,
            protoconn) < 0){
            goto error_exit;
        }
    }
ok_exit:
    res = 1;
    goto exit;
error_exit:
    res = 0;
    goto exit;
exit:
    return res;
}

MIC_THREAD_FUN_DEF(Listener_Thread,info){
    while(1){
        PCMANAGER_Traversal_Neighbor(m_Listener_For_EachAgentNode);
        MIC_USLEEP(10);
    }
    return 0;
}

int StartListenerThread(){
    MIC_THREAD_HANDLE_ID thread_id;
    if(MIC_THREAD_CREATE(thread_id,Listener_Thread,NULL)<0){
        Printf_Error("Thread id create Error");
        return 0;
    }
    MIC_USLEEP(1);
    return 1;
}

int m_callBackForEachAccept(int socket){
    pPCConn conn = PCCONN_CreatePCConnFromSocket(socket);
    int jobid;
    pAgent_proto proto = NULL;
    if(conn == PCCONN_CREATEPCCONNFROMSOCKET_ERROR){
        return 0;
    }
    if( PROTO_RECVSTATE_CANRECV == 
        PROTO_RecvState(conn)
    ){
        proto = PROTO_RecvProto(conn);
        if(proto == PROTO_RECVPROTO_ERROR){
            return 0;
        }
        if(CMDPARSE_AND_DO_ERROR == 
            CMDParse_And_Do(proto,conn)){
            return 0;
        }
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
    int sock = API_socket_connect(ip,port);
    if(sock == SOCKET_CONNECT_ERROR){
        return AGENT_CONVERSATIONPROXY_CONNECT_ERROR;
    }
    if(AGENTCONN_INTERACTIVE_OK != 
        AgentInfo_Interactive(sock,ip,port)){
        return AGENT_CONVERSATIONPROXY_CONNECT_ERROR;
    }
    return AGENT_CONVERSATIONPROXY_CONNECT_OK;
}

