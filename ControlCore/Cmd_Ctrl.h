#ifndef _CMD_CTRL_H_
#define _CMD_CTRL_H_

#include "../generic.h"
#include "../AgentManager/GlobalValue.h"

// 指令协议
// connect agent
#define AGENT_CONN_COMMAND_LISTEN        1
#define AGENT_CONN_COMMAND_CONNECT       2
// lcx
#define AGENT_SERVER_COMMAND_LCXTRAN     3
#define AGENT_SERVER_COMMAND_REVLCXTRAN  4
// file 
#define AGENT_SERVER_COMMAND_UPFILE      5
#define AGENT_SERVER_COMMAND_DOWNFILE    6
// socks
#define AGENT_SERVER_COMMAND_SOCKS       7
#define AGENT_SERVER_COMMAND_RVSOCKS     8
// shell
#define AGENT_SERVER_COMMAND_CMDSHELL    9
// send msg
#define AGENT_SERVER_COMMAND_SENDMSG    10
// +-------------------+-------------------+
// | CCTRL_TYPE(1byte) | CCTRL_ARGS(1byte) |
// +-------------------+-------------------+
// | LCX? SOCKS?       | args              |
// +-------------------+-------------------+

#define MAX_CCPROXY_LEN               200
#define CCTRL_TYPE_OFFSET             0


#define CMDCTRL_BUILDTARGETSOCK_ERROR NULL
pPCConn CMDCTRL_BuildTargetSock(int targetid,int ccproxy_cmd);


#define CMDCTRL_ONNEWTUNNEL_ERROR   -1
#define CMDCTRL_ONNEWTUNNEL_OK       1
int CMDCTRL_onNewTunnel(pPCConn conn);

#endif
