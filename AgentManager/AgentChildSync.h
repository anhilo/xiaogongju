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

#endif
