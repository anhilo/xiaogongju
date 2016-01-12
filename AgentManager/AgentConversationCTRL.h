#ifndef _AGENT_CONVERSATION_CTRL_H_
#define _AGENT_CONVERSATION_CTRL_H_
#include "../generic.h"
#include "GlobalValue.h"

typedef int (*pfun_For_NewTunnel)(int socket);
#define AGENT_TYPE_IAM_ADMIN   1
#define AGENT_TYPE_NORMALAGENT 2

#define AGENT_CONVERSATION_INIT_ERROR -1
#define AGENT_CONVERSATION_INIT_OK     1
int AGENT_Conversation_Init(int ostype,char *pcname,int agent_type);

#define AGENT_CONVERSATION_LISTEN_ERROR -1
#define AGENT_CONVERSATION_LISTEN_OK     1
int AGENT_Conversation_Listen(int port,int maxnum);

#define AGENT_CONVERSATION_CONNECT_ERROR   -1
#define AGENT_CONVERSATION_CONNECT_OK       1
int AGENT_Conversation_Connect(char *ip,int port);

#define AGENT_CONVERSATION_BUILD_SOCKTUNNEL_ERROR NULL
pPCConn AGENT_Conversation_Build_SockTunnel(int targetid);


#define AGENT_CONVERSATION_GETID_ERROR  -1
int AGENT_Conversation_GetID();

#endif
