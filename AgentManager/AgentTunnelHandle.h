#ifndef _AGENTTUNNEL_HANDLE_H_
#define _AGENTTUNNEL_HANDLE_H_

#include "../generic.h"
#include "PCConn.h"

typedef int (*pfun_CBF_Tunnel)(pPCConn conn,char *args);

#define AGENT_TUNN_INIT_ERROR   -1
#define AGENT_TUNN_INIT_OK       1
int AGENT_TUNN_Init(pfun_CBF_Tunnel fun);

/// return socket for build
#define AGENT_TUNN_BUILDTUNNEL_ERROR  NULL
pPCConn AGENT_TUNN_BuildTunnel(int targetid);

#define ON_TUNNEL_ASK_ERROR  0
#define ON_TUNNEL_ASK_OK     1 
int on_reverse_Tunnel_Ask(pAgent_proto proto,pPCConn conn);

// DIRECT Connect callback
#define ON_NEW_TUNNEL_ASK_OK    1
#define ON_NEW_TUNNEL_ASK_ERROR 0
int on_new_tunnel_ask(pPCConn conn);

// no use
//#define ON_TUNNEL_REPLY_ERROR  0
//#define ON_TUNNEL_REPLY_OK     1
//int on_reverse_Reply(int sock);

#endif
