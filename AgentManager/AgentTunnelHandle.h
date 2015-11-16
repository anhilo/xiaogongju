#ifndef _AGENTTUNNEL_HANDLE_H_
#define _AGENTTUNNEL_HANDLE_H_

int AGENT_TUNN_BuildTunnel(int to_id);

int on_Tunnel_Ask(int sock);

int on_Tunnel_Reply(int sock);

#endif
