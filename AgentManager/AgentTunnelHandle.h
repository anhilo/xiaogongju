#ifndef _AGENTTUNNEL_HANDLE_H_
#define _AGENTTUNNEL_HANDLE_H_

#include "../generic.h"
#include "PCConn.h"

/// return socket for build
#define AGENT_TUNN_BUILDTUNNEL_ERROR  NULL
pPCConn AGENT_TUNN_BuildTunnel(int to_id);

#define ON_TUNNEL_ASK_ERROR  0
#define ON_TUNNEL_ASK_OK     1 
int on_reverse_Tunnel_Ask(int sock,char *ip,int port);

#define ON_NEW_TUNNEL_ASK_OK    1
#define ON_NEW_TUNNEL_ASK_ERROR 0
int on_new_tunnel_ask(int sock);

#define ON_TUNNEL_REPLY_ERROR  0
#define ON_TUNNEL_REPLY_OK     1
int on_reverse_Reply(int sock);

#endif
