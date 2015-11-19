#ifndef _AGENTMSGHANDLE_H_
#define _AGENTMSGHANDLE_H_

#include "../generic.h"

// Error return -1
int ASK_NEW_ID();
int SendUpStream   (char *msg,int msglen);
int SendDownStream (char *msg,int msglen);

int on_UpStreamMsg_Arrive   (int sock);
int on_DownStreamMsg_Arrive (char *msg,int msglen);


#endif
