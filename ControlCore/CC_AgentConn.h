#ifndef _CC_AGENTCONN_H_
#define _CC_AGENTCONN_H_

#include "../generic.h"

#define NODETYPE_ADMIN_NODE  1
#define NODETYPE_NORMAL_NODE 2

#define OSTYPE_LINUX_OS       1
#define OSTYPE_ARMLINUX_OS    2
#define OSTYPE_WINDOW_OS      3
#define OSTYPE_MACOSX_OS      4
#define OSTYPE_UNKNOW_OS      5


#define CC_AGENT_INIT_ERROR   -1
#define CC_AGENT_INIT_OK       1
int CC_Agent_Init(int ostype,char *pcname,int node_type);

#define CC_AGENT_LISTEN_ERROR   -1
#define CC_AGENT_LISTEN_OK       1
int CC_Agent_Listen(int target,int lport,int maxnum);

#define CC_AGENT_CONNECT_ERROR   -1
#define CC_AGENT_CONNECT_OK       1
int CC_Agent_Connect(int target,char *remoteip,int rport);

#define CC_AGENT_SENDMSG_ERROR    -1
#define CC_AGENT_SENDMSG_OK        1
int CC_Agent_SendMsg(int target,char *msg,int msglen);

#define CC_AGENT_STARTSHELL_ERROR  -1
#define CC_AGENT_STARTSHELL_OK      1
int CC_Agent_StartShell(int target,int lport,int maxnum,int usec);

#define CC_AGENT_LCXTRAN_ERROR     -1
#define CC_AGENT_LCXTRAN_OK         1
int CC_Agent_LcxTran(int target,int lport,char *rip,int rport,int usec);
#endif
