#ifndef _AGENTMSGHANDLE_H_
#define _AGENTMSGHANDLE_H_

#include "../generic.h"

// Error return -1
// 向本地或上游请求新ID
int ASK_NEW_ID();

int SendAgentInfo(int fatherid,int childid,int ostype,char *pcname);

int SendUpperReplaceID(int oldid,int newid);

int SendDown_BroadCast(char *msg,int msglen);
int SendDown_DirectMsg(int targetid,char *msg,int msglen);

int Broadcast_ReplaceID(int oldid,int newid);

int on_UpStreamMsg_Arrive   (int sock);
int on_DownStreamMsg_Arrive (int sock);


#endif
