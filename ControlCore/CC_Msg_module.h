#ifndef _CC_MSG_MODULE_H_
#define _CC_MSG_MODULE_H_

#include "../generic.h"
#include "../AgentManager/GlobalValue.h"

#define MAX_MSG_LEN   300
#define CCMSG_SENDMSG_ERROR 0
#define CCMSG_SENDMSG_OK    1
int CCMSG_SendMsg(int targetid,char *msg,int msglen);

int m_onRecvMsg(pPCConn conn);
#endif
