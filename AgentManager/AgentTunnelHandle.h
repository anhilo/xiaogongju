#ifndef _AGENTTUNNEL_HANDLE_H_
#define _AGENTTUNNEL_HANDLE_H_

#include "../generic.h"

/// return socket for build
#define AGENT_TUNN_BUILDTUNNEL_ERROR  -1
int AGENT_TUNN_BuildTunnel(int to_id);

#define ON_TUNNEL_ASK_ERROR  0
#define ON_TUNNEL_ASK_OK     1 
int on_Tunnel_Ask(int sock);

#define ON_TUNNEL_REPLY_ERROR  0
#define ON_TUNNEL_REPLY_OK     1
int on_Tunnel_Reply(int sock);

#endif
