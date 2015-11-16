#ifndef _AGENTCMDINFO_h_
#define _AGENTCMDINFO_h_

#include "../generic.h"

#define CMD_PROTO_MAX_LEN      20

#define CMD_HEAD_SEND_MESSAGE   1
#define CMD_HEAD_NEW_TUNNEL     2
#define CMD_HEAD_BROADCAST      4
typedef struct CMDINFO{
    char cmd;      // 1 byte
    int fromid;    // 4 byte
    int toid;      // 4 byte
    char datalen;  // 1 byte
    char data[CMD_PROTO_MAX_LEN - 10];
}CmdInfo,*pCmdInfo;

#define AGENTCMD_PARSE_ERROR NULL
pCmdInfo AGENTCMD_parse(char *cmdbuf);

#define AGENTCMD_BUILDCMDINFO_ERROR  -1
#define AGENTCMD_BUILDCMDINFO_OK      1
int AGENTCMD_CmdInfo(pCmdInfo cmd,char *cmdBuf);

#define AGENTCMD_SENDMSG_ERROR       -1
#define AGENTCMD_SENDMSG_OK           1
int AGENTCMD_sendMSG(int sock,pCmdInfo cmd);


#endif
