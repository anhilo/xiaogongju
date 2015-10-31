#ifndef _AGENT_INTERACTIVE_H_
#define _AGENT_INTERACTIVE_H_

#include "../generic.h"

typedef int (*CBF_NewNode)     (int sock);
typedef int (*CBF_TunnelAsk)   (int sock);
typedef int (*CBF_TunnelAnswer)(int sock);
typedef int (*CBF_BroadCast)   (int sock);

// A. 初始化部分
#define AGENTINT_INIT_ERROR    -1
#define AGENTINT_INIT_OK        1
int AGENTINT_Init(
        CBF_NewNode      cbf_newnode,
        CBF_TunnelAsk    cbf_tunnelAsk,
        CBF_TunnelAnswer cbf_tunnelAnswer,
        CBF_BroadCast    cbf_broadcast
);

// B. 以某一端口创建接收服务器
#define AGENTINT_STARTSERVER_ERROR -1
#define AGENTINT_STARTSERVER_OK     1
int AGENTINT_StartServer(int port,int max);

// C. 连接服务器
//      成功返回socket
#define AGENTINT_CONNECT_ERROR     -1
int AGENTINT_Connect_IAM_NEW_NODE (char *url,int port);
int AGENTINT_Connect_NEWTUNNEL_ASK(char *url,int port);
int AGENTINT_Connect_TUNNEL_ANSWER(char *url,int port);
int AGENTINT_Connect_BroadCastSock(char *url,int port);

// D. 新会话建立
#define AGENTINT_BUILD_TUNNEL_ERROR   -1
int AGENTINT_Build_Tunnel(char *url,int port,int targetid);
#define AGENTINT_BUILD_TUNNEL_ERROR   -1
int AGENTINT_Re_Build_Tunnel(int cmd_socket,int targetid);


#endif
