#ifndef _AGENTCHILDSYNC_H_
#define _AGENTCHILDSYNC_H_

#include "../generic.h"
#include "AgentProtocol.h"

#define CHILDNODEINFOSYNCTRIGGER_ERROR -1
#define CHILDNODEINFOSYNCTRIGGER_OK     1
int ChildNodeInfoSyncTrigger();

#define ON_SYNCINFOUPPER_ERROR  -1
#define ON_SYNCINFOUPPER_OK      1
int on_SyncInfoUpper(pAgent_proto proto);

#define SYNC_SENDCHILDINFOUPPER_ERROR  -1
#define SYNC_SENDCHILDINFOUPPER_OK      1
int SYNC_SendChildInfoUpper(int fatherid,int childid, int ostype,char *pcname);



#endif
