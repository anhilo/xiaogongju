#ifndef _CC_SHELLCTRL_H_
#define _CC_SHELLCTRL_H_

#include "../generic.h"

#define CC_STARTSHELL_ERROR   -1
#define CC_STARTSHELL_OK       1
int CC_StartShell(int targetid,int locolport);

int CC_onStartShellhere(pPCConn conn);

#endif
