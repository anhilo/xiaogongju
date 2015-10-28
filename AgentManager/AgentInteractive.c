#include"AgentInteractive.h"

//*********************************
CBF_NewNode   cb_NewNode   = NULL;
CBF_TunnelAsk cb_TunnelAsk = NULL;
CBF_BroadCast cb_BroadCast = NULL;
#define AGENTINT_MAX_PROTOL_LEN 10
// +-------------+-------------------------+
// | CMD_HEAD    | DATA                    |
// +-------------+-------------------------+
// | NEWNODE_CMD | CMDMSG_NEW_NODE_CONNECT |
// |             | CMDMSG_NEW_TUNNEL_ASK   |
// |             | CMDMSG_BROADCAST_MSG    |
// +-------------+-------------------------+
// | NEW_TUNNEL  | CMDMSG_BUILDTUNNEL      |
// |             | CMDMSG_RC_BUILDTUNNEL   |
// +-------------+-------------------------+
#define AGENTINT_CMD_HEAD_OFFSET 0
#define AGENTINT_DATA_OFFSET     1
    #define NEWNODE_CMD          1
#define CMDMSG_NEW_NODE_CONNECT  1
#define CMDMSG_NEW_TUNNEL_ASK    2
#define CMDMSG_BROADCAST_MSG     3
#define NEWNODE_IAM_CLIENT            1
#define NEWNODE_IAM_NODE              2
#define NEWNODE_IAM_NODE_WITH_CLIENT  3
//*********************************

int AGENTINT_Init(
        CBF_NewNode   cbf_newnode,
        CBF_TunnelAsk cbf_tunnelAsk,
        CBF_BroadCast cbf_broadcast
){
    cb_NewNode   = cbf_newnode;
    cb_TunnelAsk = cbf_tunnelAsk;
    cb_BroadCast = cbf_broadcast;
    return AGENTINT_INIT_OK;
}

int m_CallBackForServer(int socket){
    if(socket == -1){
        return 0;
    }
    char cmdmsg[AGENTINT_MAX_PROTOL_LEN];
    int nrecv = API_socket_recv(socket,cmdmsg,
            AGENTINT_MAX_PROTOL_LEN);
    if(nrecv == SOCKET_RECV_ERROR){
        return 0;
    }
    switch(cmdmsg[AGENTINT_CMD_HEAD_OFFSET]){
        case CMDMSG_NEW_NODE_CONNECT:
            cb_NewNode(socket);
            break;
        case CMDMSG_NEW_TUNNEL_ASK:
            cb_TunnelAsk(socket);
            break;
        case CMDMSG_BROADCAST_MSG:
            cb_BroadCast(socket);
            break;
    }
    return 1;
}

int AGENTINT_StartServer(int port,int max){
    int sock = API_socket_init_server(port,max);
    if(sock != SOCKET_ERROR){
        int res = API_socket_server_start(sock,
                    m_CallBackForServer);
        if(res != API_SOCKET_SERVER_START_ERROR){
            return AGENTINT_STARTSERVER_OK;
        }
    }
    return AGENTINT_STARTSERVER_ERROR;
}

int agent_newnode_connect(char *url,int port,int cmdmsg){
    char cmdbuf [ AGENTINT_MAX_PROTOL_LEN ];
    cmdbuf[ AGENTINT_CMD_HEAD_OFFSET ] = cmdmsg;
    int sock = API_socket_connect(url,port);
    if(sock == SOCKET_ERROR){
        return AGENTINT_CONNECT_ERROR;
    }
    int res = API_socket_send(sock,cmdbuf,
        AGENTINT_MAX_PROTOL_LEN);
    if(res == SOCKET_SEND_ERROR 
        || res != AGENTINT_MAX_PROTOL_LEN){
        return AGENTINT_CONNECT_ERROR;
    }
    return sock;
}

int AGENTINT_Connect_IAM_CLIENT(char *url,int port){
    return agent_newnode_connect(
            url,
            port,
            NEWNODE_IAM_CLIENT);
}

int AGENTINT_Connect_IAM_NODE(char *url,int port){
    return agent_newnode_connect(
            url,
            port,
            NEWNODE_IAM_NODE);
}

int AGENTINT_Connect_IAM_NODE_WITH_CLIENT(char *url,int port){
    return agent_newnode_connect(
            url,
            port,
            NEWNODE_IAM_NODE_WITH_CLIENT);
}
