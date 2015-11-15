#ifndef _AGENT_CONVERSATION_PROXY_H_
#define _AGENT_CONVERSATION_PROXY_H_

#include "../generic.h"
#define AGENT_CONVERSATIONPROXY_INIT_ERROR -1
#define AGENT_CONVERSATIONPROXY_INIT_OK     1
int AGENT_ConversationProxy_Init();

#define AGENT_CONVERSATIONPROXY_STARTSERVER_ERROR -1
#define AGENT_CONVERSATIONPROXY_STARTSERVER_OK     1
int AGENT_ConversationProxy_StartServer(int port,int maxnum);

#define AGENT_CONVERSATIONPROXY_CONNECT_ERROR -1
#define AGENT_CONVERSATIONPROXY_CONNECT_OK     1
int AGENT_ConversationProxy_Connect(char *ip,int port);

#define AGENT_CONVERSATIONPROXY_BUILD_TUNNEL_ERROR -1
int AGENT_ConversationProxy_Build_Tunnel(int targetid);

#endif
